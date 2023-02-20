/* main.c - Application main entry point */

/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/net/buf.h>

#include <zcbor_encode.h>
#include <zcbor_decode.h>
#include <zcbor_common.h>

#include <zephyr/sys/byteorder.h>
#include <dk_buttons_and_leds.h>

#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>


#include <smp/smp.h>
#include "mgmt/mgmt.h"
#include "img_mgmt/img_mgmt.h"

#include <pm_config.h>

#include <zephyr/mgmt/mcumgr/serial.h>
#include <zephyr/drivers/console/uart_mcumgr.h>

#include <string.h>

/* Mimimal number of ZCBOR encoder states to provide full encoder functionality. */
#define CBOR_ENCODER_STATE_NUM 2

/* Number of ZCBOR decoder states required to provide full decoder functionality plus one
 * more level for decoding nested map received in response to SMP echo command.
 */
#define CBOR_DECODER_STATE_NUM 3

#define CBOR_MAP_MAX_ELEMENT_CNT 2
#define CBOR_BUFFER_SIZE 128

#define SMP_ECHO_MAP_KEY_MAX_LEN 2
#define SMP_ECHO_MAP_VALUE_MAX_LEN 30

#define KEY_LIST_MASK  DK_BTN1_MSK
#define KEY_UPLOAD_MASK  DK_BTN2_MSK
#define KEY_TEST_MASK  DK_BTN3_MSK
#define KEY_CONFIRM_MASK  DK_BTN4_MSK


struct k_sem upload_sem;
K_SEM_DEFINE(upload_sem, 1, 1);

static char hash_value_secondary_slot[33];
static char hash_value_primary_slot[33];

static void smp_uart_process_rx_queue(struct k_work *work);
void send_upload2(struct k_work *work);

K_FIFO_DEFINE(smp_uart_rx_fifo_custom);
K_WORK_DEFINE(smp_uart_work_custom, smp_uart_process_rx_queue);
K_WORK_DEFINE(upload_work_item, send_upload2);

static void smp_upload_handler(struct net_buf *nb, struct mgmt_hdr *hdr)
{
    void *new_ptr = net_buf_pull(nb, sizeof(struct mgmt_hdr));

    zcbor_state_t zsd[CBOR_DECODER_STATE_NUM];
    struct zcbor_string value = {0};
    bool ok;
    zcbor_new_decode_state(zsd, ARRAY_SIZE(zsd), new_ptr, nb->len, 1);

    /* Stop decoding on the error. */
    zsd->constant_state->stop_on_error = true;

    ok = zcbor_map_start_decode(zsd);
    if (!ok) {
        printk("Decoding error, start_decode (err: %d)\n", zcbor_pop_error(zsd));
        return;
    } 

    /**
     * 
     * Decoding rc (status error code)
     * 
     */

    //Decoding rc key
    char rc_key[5];
    ok = zcbor_tstr_decode(zsd, &value);
    if (!ok) {
        printk("Decoding error, rc key (err: %d)\n", zcbor_pop_error(zsd));
        return;
    }  else if (value.len != 2) {
        printk("Invalid data received (rc key). Length %d is not equal 2\n", value.len);
        return;
    } 
    memcpy(rc_key, value.value, value.len);
    rc_key[value.len] = '\0';
    // printk("rc key: [%s]\n",rc_key);

    //Decoding rc value
    int32_t rc_value;
    ok = zcbor_int32_decode(zsd, &rc_value);
    if (!ok) {
        printk("Decoding error, rc value (err: %d)\n", zcbor_pop_error(zsd));
        return;
    };
    if(rc_value){
        printk("Error in image upload response: %d\n", rc_value);
        return;
    }
    // printk("rc_value: %d\n",rc_value);

    //decoding offset key
    char off_key[5];
    ok = zcbor_tstr_decode(zsd, &value);
    if (!ok) {
        printk("Decoding error, offset key (err: %d)\n", zcbor_pop_error(zsd));
        return;
    } else if ((value.len != 3)) {
        printk("Invalid data received (rc key). Length %d is not equal 3\n", value.len);
        return;
    }

    memcpy(off_key, value.value, value.len);
    off_key[value.len] = '\0';
    // printk("off_key [%s]\n",off_key);

    //Decoding offset value
    int32_t off_val;
    ok = zcbor_int32_decode(zsd, &off_val);
    if (!ok) {
        printk("Decoding error, offset value (err: %d)\n", zcbor_pop_error(zsd));
        return;
    }
    zcbor_map_end_decode(zsd);
    // printk("off_val: %d\n",off_val);


    if (zcbor_check_error(zsd)) {
        //printk("%s: %d\n", off_key, off_val);
        //do nothing
    } else {
        printk("Cannot print received image upload CBOR stream (err: %d)\n",
                zcbor_pop_error(zsd));
    }
    k_work_submit(&upload_work_item);
}

#define PROGRESS_WIDTH 50
static void progress_print(size_t downloaded, size_t file_size)
{
    const int percent = (downloaded * 100) / file_size;
    size_t lpad = (percent * PROGRESS_WIDTH) / 100;
    size_t rpad = PROGRESS_WIDTH - lpad;

    printk("\r[ %3d%% ] |", percent);
    for (size_t i = 0; i < lpad; i++) {
        printk("=");
    }
    for (size_t i = 0; i < rpad; i++) {
        printk(" ");
    }
    printk("| (%d/%d bytes)", downloaded, file_size);
}

#define UPLOAD_CHUNK		50 //This has to be at least 32 bytes, since first it has to send the whole header (which is 32 bytes)
void send_upload2(struct k_work *item)
{
    struct mgmt_hdr image_list_header; 
    struct net_buf *nb; 

    const struct device *flash_dev;
    uint8_t data[UPLOAD_CHUNK+1]; // One more byte, to store '/0'

    flash_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash_controller));
    // const int last_addr = PM_MCUBOOT_SECONDARY_END_ADDRESS;  
    const int last_addr = PM_MCUBOOT_SECONDARY_ADDRESS + 60000;
    const int start_addr = PM_MCUBOOT_SECONDARY_ADDRESS;
    static int curr_addr = start_addr;
    int upload_chunk = UPLOAD_CHUNK;
    int err;
    static bool update_complete = false;

    if(update_complete){
        printk("Update is complete.\n");
        update_complete = false;
        curr_addr = start_addr;
        return;
    }

    nb = smp_packet_alloc(); 

    zcbor_state_t zs[CBOR_ENCODER_STATE_NUM]; 
    zcbor_new_encode_state(zs, ARRAY_SIZE(zs), nb->data + sizeof(struct mgmt_hdr), net_buf_tailroom(nb), 0); 
    //Wait until response is received until sending the next chunk
    if(err){
        printk("Error uploading. sem ret: %d\n", err);
        goto end;
    }

    if(curr_addr+UPLOAD_CHUNK > last_addr){
        upload_chunk = last_addr - curr_addr;
        update_complete = true;
    }
    progress_print(curr_addr-start_addr, last_addr-start_addr);
    err = flash_read(flash_dev, curr_addr, data, upload_chunk);
    if (err != 0) {
        printk("flash_read failed with error: %d\n", err);
        goto end;
    }

    data[upload_chunk] = '\0';
    zs->constant_state->stop_on_error = true;
    zcbor_map_start_encode(zs, 20);
    zcbor_tstr_put_lit(zs, "image");
    zcbor_int64_put(zs, 0);
    zcbor_tstr_put_lit(zs, "data");
    zcbor_bstr_put_lit(zs, data);
    zcbor_tstr_put_lit(zs, "len");
    zcbor_uint64_put(zs, (uint64_t)0x5B68);
    zcbor_tstr_put_lit(zs, "off");
    zcbor_uint64_put(zs, curr_addr - start_addr);
    zcbor_tstr_put_lit(zs, "sha");
    zcbor_bstr_put_lit(zs, "12345");
    zcbor_tstr_put_lit(zs, "upgrade");
    zcbor_bool_put(zs, false);
    zcbor_map_end_encode(zs, 20);

    if (!zcbor_check_error(zs)) {
        printk("Failed to encode SMP test packet, err: %d\n", zcbor_pop_error(zs));
        goto end;
    }
    curr_addr+=upload_chunk;

    //payload_len = (size_t)(zs->payload - smp_cmd.payload);
    //
    image_list_header.nh_op = MGMT_OP_WRITE; 
    image_list_header.nh_flags = 0; 
    image_list_header.nh_len = zs->payload_mut - nb->data - MGMT_HDR_SIZE; 
    image_list_header.nh_group = MGMT_GROUP_ID_IMAGE; 
    image_list_header.nh_seq = 0; 
    image_list_header.nh_id  = IMG_MGMT_ID_UPLOAD; 
    mgmt_hton_hdr(&image_list_header); 

    nb->len = zs->payload_mut - nb->data;
    memcpy(nb->data, &image_list_header, sizeof(image_list_header)); 

    // printk("nb->len %d\n",nb->len);
    // printk("image_list_header.nh_len %d\n", image_list_header.nh_len);

    err = uart_mcumgr_send(nb->data, nb->len); 
    if(err){
        printk("upload2: uart_mcumgr_send failed with %d\n", err);
        goto end;
    }
end: 
    net_buf_unref(nb);
}


static int send_smp_list()
{
    struct mgmt_hdr image_list_header; 
    struct net_buf *nb; 

    nb = smp_packet_alloc(); 

    zcbor_state_t zs[CBOR_ENCODER_STATE_NUM]; 
    zcbor_new_encode_state(zs, 2, nb->data + sizeof(struct mgmt_hdr), net_buf_tailroom(nb), 0); 
    zcbor_map_start_encode(zs, 1);
    zcbor_map_end_encode(zs, 1);

    image_list_header.nh_op = MGMT_OP_READ; 
    image_list_header.nh_flags = 0; 
    image_list_header.nh_len = 0; 
    image_list_header.nh_group = MGMT_GROUP_ID_IMAGE; 
    image_list_header.nh_seq = 0; 
    image_list_header.nh_id  = IMG_MGMT_ID_STATE; 
    mgmt_hton_hdr(&image_list_header); 

    nb->len = sizeof(image_list_header); 
    memcpy(nb->data, &image_list_header, sizeof(image_list_header)); 

    uart_mcumgr_send(nb->data, nb->len); 
    net_buf_unref(nb);
    return 0;
}

static int send_smp_confirm()
{
    struct mgmt_hdr image_list_header; 
    struct net_buf *nb; 

    nb = smp_packet_alloc(); 

    zcbor_state_t zs[CBOR_ENCODER_STATE_NUM]; 
    zcbor_new_encode_state(zs, 2, nb->data + sizeof(struct mgmt_hdr), net_buf_tailroom(nb), 0); 

    /* Stop encoding on the error. */
    zs->constant_state->stop_on_error = true;

    zcbor_map_start_encode(zs, CBOR_MAP_MAX_ELEMENT_CNT);
    zcbor_tstr_put_lit(zs, "hash");
    zcbor_bstr_put_lit(zs, hash_value_secondary_slot);
    zcbor_tstr_put_lit(zs, "confirm");
    zcbor_bool_put(zs, true);

    zcbor_map_end_encode(zs, CBOR_MAP_MAX_ELEMENT_CNT);

    if (!zcbor_check_error(zs)) {
        printk("Failed to encode SMP confirm packet, err: %d\n", zcbor_pop_error(zs));
        return -EFAULT;
    }

    image_list_header.nh_op = MGMT_OP_WRITE; 
    image_list_header.nh_flags = 0; 
    image_list_header.nh_len = zs->payload_mut - nb->data -MGMT_HDR_SIZE; 
    image_list_header.nh_group = MGMT_GROUP_ID_IMAGE; 
    image_list_header.nh_seq = 0; 
    image_list_header.nh_id  = IMG_MGMT_ID_STATE; 
    mgmt_hton_hdr(&image_list_header); 

    nb->len = zs->payload_mut - nb->data;
    memcpy(nb->data, &image_list_header, sizeof(image_list_header)); 

    // confirm has same response as list command
    uint8_t ret = uart_mcumgr_send(nb->data, nb->len); 
    net_buf_unref(nb);
    return ret;
}


static int send_smp_test()
{
    struct mgmt_hdr image_list_header; 
    struct net_buf *nb; 
    zcbor_state_t zs[CBOR_ENCODER_STATE_NUM]; 

    nb = smp_packet_alloc(); 

    zcbor_new_encode_state(zs, 2, nb->data + sizeof(struct mgmt_hdr), net_buf_tailroom(nb), 0); 

    /* Stop encoding on the error. */
    zs->constant_state->stop_on_error = true;

    zcbor_map_start_encode(zs, CBOR_MAP_MAX_ELEMENT_CNT);
    zcbor_tstr_put_lit(zs, "hash");
    zcbor_bstr_put_lit(zs, hash_value_secondary_slot);
    zcbor_tstr_put_lit(zs, "confirm");
    zcbor_bool_put(zs, false); // From SMP Protocol, confirm=false means Test

    zcbor_map_end_encode(zs, CBOR_MAP_MAX_ELEMENT_CNT);

    if (!zcbor_check_error(zs)) {
        printk("Failed to encode SMP confirm packet, err: %d\n", zcbor_pop_error(zs));
        return -EFAULT;
    }


    image_list_header.nh_op = MGMT_OP_WRITE; 
    image_list_header.nh_flags = 0; 
    image_list_header.nh_len = zs->payload_mut - nb->data - MGMT_HDR_SIZE; 
    image_list_header.nh_group = MGMT_GROUP_ID_IMAGE; 
    image_list_header.nh_seq = 0; 
    image_list_header.nh_id  = IMG_MGMT_ID_STATE; 
    mgmt_hton_hdr(&image_list_header); 

    nb->len = zs->payload_mut - nb->data;
    memcpy(nb->data, &image_list_header, sizeof(image_list_header)); 

    // confirm has same response as list command
    uint8_t ret = uart_mcumgr_send(nb->data, nb->len); 
    net_buf_unref(nb);
    return ret;
}

static void button_upload(bool state)
{
    if (state) {
        k_work_submit(&upload_work_item);
    }
}


static void button_confirm(bool state)
{
    if (state) {
        int ret;
        ret = send_smp_confirm();
        if (ret) {
            printk("Confirm command send error (err: %d)\n", ret);
        }
    }
}

static void button_test(bool state)
{
    if (state) {
        int ret;
        ret = send_smp_test();
        if (ret) {
            printk("Test command send error (err: %d)\n", ret);
        }
    }
}

static void button_image_list(bool state)
{
    if (state) {
        int ret;

        ret = send_smp_list();
        if (ret) {
            printk("Image list command send error (err: %d)\n", ret);
        }
    }
}


static void button_handler(uint32_t button_state, uint32_t has_changed)
{
    if (has_changed & KEY_LIST_MASK) {
        button_image_list(button_state & KEY_LIST_MASK);
    }
    if(has_changed & KEY_UPLOAD_MASK){
        button_upload(button_state & KEY_UPLOAD_MASK);
    }
    if(has_changed & KEY_TEST_MASK){
        button_test(button_state & KEY_TEST_MASK);
    }
    if(has_changed & KEY_CONFIRM_MASK){
        button_confirm(button_state & KEY_CONFIRM_MASK);
    }
    //No more buttons for reset
}

static struct mcumgr_serial_rx_ctxt smp_uart_rx_ctxt;

    static int
smp_read_hdr(const struct net_buf *nb, struct mgmt_hdr *dst_hdr)
{
    if (nb->len < sizeof(*dst_hdr)) {
        return MGMT_ERR_EINVAL;
    }

    memcpy(dst_hdr, nb->data, sizeof(*dst_hdr));
    return 0;
}

void smp_list_handler(struct net_buf *nb)
{
    /* Skip the mgmt_hdr */
    void *new_ptr = net_buf_pull(nb, sizeof(struct mgmt_hdr));

    zcbor_state_t zsd[10];
    struct zcbor_string value = {0};
    bool ok;
    //printk("nb->len: %d\n",nb->len);

    zcbor_new_decode_state(zsd, ARRAY_SIZE(zsd), new_ptr, nb->len, 1);

    /* Stop decoding on the error. */
    zsd->constant_state->stop_on_error = true;

    ok = zcbor_map_start_decode(zsd);
    if (!ok) {
        printk("Decoding error 1, start_decode (err: %d)\n", zcbor_pop_error(zsd));
        return;
    } 

    //Decoding images key
    char images_key[10];
    ok = zcbor_tstr_decode(zsd, &value);
    if (!ok) {
        printk("Decoding error 2, images key (err: %d)\n", zcbor_pop_error(zsd));
        return;
    }  /*else if (value.len != 6) {
         printk("Invalid data received (images key). Length %d is not equal 2\n", value.len);
         return;
         } else if(!strncmp(value.value, 'images', 6)){
         printk("Invalid data received (images key). String '%.2s' is not equal to 'images'\n", value.value);
         return;
         }*/
    memcpy(images_key, value.value, value.len);
    images_key[value.len] = '\0';
    printk("Images key: %s\n",images_key);
    ok = zcbor_list_start_decode(zsd);
    if (!ok) {
        printk("Decoding error, start_decode images->list  (err: %d)\n", zcbor_pop_error(zsd));
        return;
    } 


    for(int slot=0; slot<2;slot++){
        ok = zcbor_map_start_decode(zsd);
        if (!ok) {
            if(slot == 0){
                printk("Error decoding slot 0. Err: %d", zcbor_pop_error(zsd));
            }else if(slot == 1){
                //printk("No secondary image present\n");
                break;
            }
        } 
        if(slot==0){
            printk("\n-----------PRIMARY IMAGE-----------\n");
        }else if(slot == 1){
            printk("\n-----------SECONDARY IMAGE-----------\n");
        }

        //Decoding slot key 
        char slot_key[5];
        ok = zcbor_tstr_decode(zsd, &value);
        if (!ok) {
            printk("Decoding error, slot key (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }  /*else if (value.len != 6) {
             printk("Invalid data received (images key). Length %d is not equal 2\n", value.len);
             return;
             } else if(!strncmp(value.value, 'images', 6)){
             printk("Invalid data received (images key). String '%.2s' is not equal to 'images'\n", value.value);
             return;
             }*/
        memcpy(slot_key, value.value, value.len);
        slot_key[value.len] = '\0';
        //printk("Slot key: %s\n",slot_key);

        //Decoding slot value
        int32_t slot_value;
        ok = zcbor_int32_decode(zsd, &slot_value);
        if (!ok) {
            printk("Decoding error, slot value (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }
        printk("      %s: %d\n",slot_key,slot_value);

        //Decoding version key
        char version_key[5];
        ok = zcbor_tstr_decode(zsd, &value);
        if (!ok) {
            printk("Decoding error, version key (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }  /*else if (value.len != 6) {
             printk("Invalid data received (images key). Length %d is not equal 2\n", value.len);
             return;
             } else if(!strncmp(value.value, 'images', 6)){
             printk("Invalid data received (images key). String '%.2s' is not equal to 'images'\n", value.value);
             return;
             }*/
        memcpy(version_key, value.value, value.len);
        version_key[value.len] = '\0';
        //printk("version key: %s\n",version_key);

        //decoding version value
        char version_value[5];
        ok = zcbor_tstr_decode(zsd, &value);
        if (!ok) {
            printk("Decoding error, version value (err: %d)\n", zcbor_pop_error(zsd));
            return;
        } /*else if ((value.len != 3)) {
            printk("Invalid data received (rc key). Length %d is not equal 3\n", value.len);
            return;
            }*/
        memcpy(version_value, value.value, value.len);
        version_value[value.len] = '\0';
        printk("      %s: %s\n",version_key,version_value);

        //Decoding hash key
        char hash_key[5];
        ok = zcbor_tstr_decode(zsd, &value);
        if (!ok) {
            printk("Decoding error, hash key (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }  /*else if (value.len != 6) {
             printk("Invalid data received (images key). Length %d is not equal 2\n", value.len);
             return;
             } else if(!strncmp(value.value, 'images', 6)){
             printk("Invalid data received (images key). String '%.2s' is not equal to 'images'\n", value.value);
             return;
             }*/
        memcpy(hash_key, value.value, value.len);
        hash_key[value.len] = '\0';
        //printk("hash key: %s\n",hash_key);

        //decoding hash value
        char hash_value[40];
        ok = zcbor_bstr_decode(zsd, &value);
        if (!ok) {
            printk("Decoding error, hash value (err: %d)\n", zcbor_pop_error(zsd));
            return;
        } /*else if ((value.len != 3)) {
            printk("Invalid data received (rc key). Length %d is not equal 3\n", value.len);
            return;
            }*/
        memcpy(hash_value, value.value, value.len);
        if(slot == 0){
            memcpy(hash_value_primary_slot, value.value, value.len);
        }
        else if(slot == 1){
            memcpy(hash_value_secondary_slot, value.value, value.len);
        }
        hash_value[value.len] = '\0';
        printk("      %s: 0x", hash_key);
        for(int x = 0; x< value.len;x++){
            printk("%x", hash_value[x]);
        }
        printk("\n");

        //Decoding bootable key
        char bootable_key[10];
        ok = zcbor_tstr_decode(zsd, &value);
        if (!ok) {
            printk("Decoding error, hash key (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }  /*else if (value.len != 6) {
             printk("Invalid data received (images key). Length %d is not equal 2\n", value.len);
             return;
             } else if(!strncmp(value.value, 'images', 6)){
             printk("Invalid data received (images key). String '%.2s' is not equal to 'images'\n", value.value);
             return;
             }*/
        memcpy(bootable_key, value.value, value.len);
        bootable_key[value.len] = '\0';
        //printk("bootable key: %s\n",bootable_key);

        //Decoding bootable value
        bool bootable_value;
        bootable_value = zcbor_bool_expect(zsd, true);
        if (!zcbor_check_error(zsd)) {
            printk("Decoding error, bootable value (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }
        printk("      %s: %s\n",bootable_key, bootable_value?"true":"false");

        //Decoding pending key
        char pending_key[10];
        ok = zcbor_tstr_decode(zsd, &value);
        if (!ok) {
            printk("Decoding error, pending key (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }  /*else if (value.len != 6) {
             printk("Invalid data received (images key). Length %d is not equal 2\n", value.len);
             return;
             } else if(!strncmp(value.value, 'images', 6)){
             printk("Invalid data received (images key). String '%.2s' is not equal to 'images'\n", value.value);
             return;
             }*/
        memcpy(pending_key, value.value, value.len);
        pending_key[value.len] = '\0';
        //printk("pending key: %s\n",pending_key);

        //Decoding pending value
        bool pending_value;
        pending_value = zcbor_bool_expect(zsd, true);
        if (!zcbor_check_error(zsd)) {
            printk("Decoding error, pending value (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }
        printk("      %s: %s\n",pending_key, pending_value?"true":"false");

        //Decoding confirmed key
        char confirmed_key[10];
        ok = zcbor_tstr_decode(zsd, &value);
        if (!ok) {
            printk("Decoding error, confirmed key (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }  /*else if (value.len != 6) {
             printk("Invalid data received (images key). Length %d is not equal 2\n", value.len);
             return;
             } else if(!strncmp(value.value, 'images', 6)){
             printk("Invalid data received (images key). String '%.2s' is not equal to 'images'\n", value.value);
             return;
             }*/
        memcpy(confirmed_key, value.value, value.len);
        confirmed_key[value.len] = '\0';
        //printk("Confirmed key: %s\n",confirmed_key);

        //Decoding confirmed value
        bool confirmed_value;
        confirmed_value = zcbor_bool_expect(zsd, true);
        if (!zcbor_check_error(zsd)) {
            printk("Decoding error, confirmed value (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }
        printk("      %s: %s\n",confirmed_key, confirmed_value?"true":"false");

        //Decoding active key
        char active_key[10];
        ok = zcbor_tstr_decode(zsd, &value);
        if (!ok) {
            printk("Decoding error, active key (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }  /*else if (value.len != 6) {
             printk("Invalid data received (images key). Length %d is not equal 2\n", value.len);
             return;
             } else if(!strncmp(value.value, 'images', 6)){
             printk("Invalid data received (images key). String '%.2s' is not equal to 'images'\n", value.value);
             return;
             }*/
        memcpy(active_key, value.value, value.len);
        active_key[value.len] = '\0';
        //printk("Active key: %s\n",active_key);

        //Decoding active value
        bool active_value;
        active_value = zcbor_bool_expect(zsd, true);
        if (!zcbor_check_error(zsd)) {
            printk("Decoding error, active value (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }
        printk("      %s: %s\n",active_key, active_value?"true":"false");

        //Decoding permanent key
        char permanent_key[10];
        ok = zcbor_tstr_decode(zsd, &value);
        if (!ok) {
            printk("Decoding error, permanent key (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }  /*else if (value.len != 6) {
             printk("Invalid data received (images key). Length %d is not equal 2\n", value.len);
             return;
             } else if(!strncmp(value.value, 'images', 6)){
             printk("Invalid data received (images key). String '%.2s' is not equal to 'images'\n", value.value);
             return;
             }*/
        memcpy(permanent_key, value.value, value.len);
        permanent_key[value.len] = '\0';
        //printk("Permanent key: %s\n",permanent_key);

        //Decoding permanent value
        bool permanent_value;
        permanent_value = zcbor_bool_expect(zsd, true);
        if (!zcbor_check_error(zsd)) {
            printk("Decoding error, permanent value (err: %d)\n", zcbor_pop_error(zsd));
            return;
        }
        printk("      %s: %s\n",permanent_key, permanent_value?"true":"false");


        zcbor_map_end_decode(zsd);

    }
    zcbor_list_end_decode(zsd);
    zcbor_map_end_decode(zsd);





    printk("Test end\n");

}

void smp_handler(struct net_buf *nb){

    uint8_t rc = 0;
    struct mgmt_hdr nb_hdr;

    rc = smp_read_hdr(nb, &nb_hdr);
    if(rc !=0){
        printk("Error reading net_buf header\n");
    }
    mgmt_ntoh_hdr(&nb_hdr);
    // printk("nb_hdr.nh_op: %d\n",nb_hdr.nh_op);
    // printk("nb_hdr.nh_group: %d\n",nb_hdr.nh_group);
    // printk("nb_hdr.nh_id: %d\n",nb_hdr.nh_id);

    if( nb_hdr.nh_op != MGMT_OP_READ_RSP && nb_hdr.nh_op != MGMT_OP_WRITE_RSP ){
        printk("Error: Expected SMP Response.\n");
        return;
    }

    if(nb_hdr.nh_group == MGMT_GROUP_ID_OS) {
        printk("SMP OS handler not implemented yet\n");
    }
    else if(nb_hdr.nh_group == MGMT_GROUP_ID_IMAGE) {
        switch(nb_hdr.nh_id){
            case IMG_MGMT_ID_STATE:
                smp_list_handler(nb);
                break;
            case IMG_MGMT_ID_UPLOAD:
                smp_upload_handler(nb,&nb_hdr);
                break;
            case IMG_MGMT_ID_ERASE:
                printk("SMP img erase handler not implemented yet.\n");
                break;
            default:
                printk("Error: Unexpected SMP ID.\n");
        }
    }
    else {
        printk("Error: Unexpected SMP Group.\n");
    }

    return;

}

/**
 * Processes a single line (fragment) coming from the mcumgr UART driver.
 */
static void smp_uart_process_frag(struct uart_mcumgr_rx_buf *rx_buf)
{
    struct net_buf *nb;

    /* Decode the fragment and write the result to the global receive
     * context.
     */
    nb = mcumgr_serial_process_frag(&smp_uart_rx_ctxt,
            rx_buf->data, rx_buf->length);

    // printk("rx_buf->length: %d\n",rx_buf->length);
    // printk("nb->length: %d\n",nb->len);
    /* Release the encoded fragment. */
    uart_mcumgr_free_rx_buf(rx_buf);

    /* Check if complete package has been received
    */
    if (nb == NULL) {
        printk("Waiting for more fragments\n");
        return;
    } 
    smp_handler(nb);
    smp_packet_free(nb);
    return;
}



static void smp_uart_process_rx_queue(struct k_work *work)
{
    struct uart_mcumgr_rx_buf *rx_buf;

    while ((rx_buf = k_fifo_get(&smp_uart_rx_fifo_custom, K_NO_WAIT)) != NULL) {
        smp_uart_process_frag(rx_buf);
    }
}

/**
 * Enqueues a received SMP fragment for later processing.  This function
 * executes in the interrupt context.
 */
static void smp_uart_rx_frag(struct uart_mcumgr_rx_buf *rx_buf)
{
    k_fifo_put(&smp_uart_rx_fifo_custom, rx_buf);
    k_work_submit(&smp_uart_work_custom);
}


void main(void)
{
    int err;

    printk("Starting SMP Client over UART sample\n");

    uart_mcumgr_register(smp_uart_rx_frag);

    k_work_init(&upload_work_item, send_upload2);

    err = dk_buttons_init(button_handler);
    if (err) {
        printk("Failed to initialize buttons (err %d)\n", err);
        return;
    }

    printk("Initalization done\n");
}
