# Bluetooth: Central SMP Client DFU
This example is copied from https://github.com/simon-iversen/ncs\_samples/tree/master/central\_smp\_client\_dfu.

With this sample you will be able to perform a DFU from one nRF52840DK to another nRF52840DK

## Requirements
This sample is tested with two nRF52840DK's and NCS v2.0.0

## Preparations

This patch should be applied to _< ncs location >/modules/lib/zcbor/src/zcbor_decode.c_ in order to be able to parse cbor fields. I was not able to get _zcbor_bool_decode()_ to work, but after applying the below patch I was able to use zcbor_bool_expect() to parse cbor bool fields.
```diff
diff --git a/src/zcbor_decode.c b/src/zcbor_decode.c
index 09c3aa4..ece1466 100644
--- a/src/zcbor_decode.c
+++ b/src/zcbor_decode.c
@@ -624,7 +624,8 @@ static bool primx_expect(zcbor_state_t *state, uint8_t result)
 	}
 
 	if (value != result) {
-		ERNG_R_RESTORE(ZCBOR_ERR_WROVALUE);
+		return false;
+		//ERR_RESTORE(ZCBOR_ERR_WRONG_VALUE);
 	}
 	return true;
 }
@@ -666,7 +667,7 @@ bool zcbor_bool_decode(zcbor_state_t *state, bool *result)
 bool zcbor_bool_expect(zcbor_state_t *state, bool result)
 {
 	if (!primx_expect(state, (uint8_t)(!!result) + ZCBOR_BOOL_TO_PRIM)) {
-		ZCBOR_FAIL();
+		return false;
 	}
 	return true;
 }

 ```
 
 ## Instructions for updating the nRF52840 from another nRF52840
 
 I'm using the name nRF52840DK_client for the DK where the sample central_smp_client_dfu runs and the name nRF52840DK_server where the sample smp_svr runs
 
 1. Program this sample (central_smp_client_dfu) to nRF52840DK_client
 2. Open J-Link Commander, connect to nRF52840DK_client (Use SWD and 4000 kHz)
 3. Run the following command: loadbin < location where this sample is placed > \central_smp_client_dfu\app_update.bin 0x50000
 4. Build and program the sample _\zephyr\samples\subsys\mgmt\mcumgr\smp_svr_ to nRF52840DK_server with the Kconfig fragment _\smp_svr\overlay-bt.conf_ applied
 5. Reset both of the chips and open two serial terminal to se the logs. Make sure you see "connected" from both of the 52840's. You may get an error about failed security, but it doesn't matter
 6. Press button 1 on nRF52840DK_client, to run the image list command, you should see something like this:
  
 ```
-----------PRIMARY IMAGE-----------
      slot: 0
      version: 0.0.0
      hash: 0xfe8db57ddd8f695ea11a9c626f156094818a9faf639b3580fe6e5d43a44be
      bootable: true
      pending: false
      confirmed: true
      active: true
      permanent: false
```
7. Press button 2 on nRF52840DK_client to run the upload command. The progress bar should appear:
 ```
[ 55% ] |===========================                         | (124000/222464 bytes)
```
8. After the whole binary is transferred over to nRF52840DK_server, run the image list command again. You should see something like this:

 ```
-----------PRIMARY IMAGE-----------
      slot: 0
      version: 0.0.0
      hash: 0xfe8db57ddd8f695ea11a9c626f156094818a9faf639b3580fe6e5d43a44be
      bootable: true
      pending: false
      confirmed: true
      active: true
      permanent: false

-----------SECONDARY IMAGE-----------
      slot: 1
      version: 0.0.0
      hash: 0xa54378a590f6615be1673d4162d312ef39f5c77631eb1d7a49607bafe1617eb4
      bootable: true
      pending: false
      confirmed: false
      active: false
      permanent: false
```
You can see that the pending field of the secondary image is set to false. 

9. In order to test the image on reboot, press button 3 on nRF52840DK_client. The pending field of the secondary image should now be true
  
 ```
-----------PRIMARY IMAGE-----------
      slot: 0
      version: 0.0.0
      hash: 0xfe8db57ddd8f695ea11a9c626f156094818a9faf639b3580fe6e5d43a44be
      bootable: true
      pending: false
      confirmed: true
      active: true
      permanent: false

-----------SECONDARY IMAGE-----------
      slot: 1
      version: 0.0.0
      hash: 0xa54378a590f6615be1673d4162d312ef39f5c77631eb1d7a49607bafe1617eb4
      bootable: true
      pending: true
      confirmed: false
      active: false
      permanent: false
```
10. In order to swap the images, the smp_svr needs to be reset. Press button 4 on nRF52840DK_client to reset the nRF52840DK_server. You should see the following log on the nRF52840DK_server, if the update was successful:

```
*** Booting Zephyr OS build v3.0.99-ncs1  ***
I: Starting bootloader
I: Primary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
I: Secondary image: magic=good, swap_type=0x2, copy_done=0x3, image_ok=0x3
I: Boot source: none
I: Swap type: test
I: Starting swap using move algorithm.
I: Bootloader chainload address offset: 0xc000
*** Booting Zephyr OS build v3.0.99-ncs1  ***
UPDATE SUCCESSFUL!


[00:00:00.234,039] <inf> smp_bt_sample: Connected
```
  
The function boot_write_img_confirmed() is added to main() of the smp_svr update, so if you reset the nRF52840DK_server again, it won't revert back.
