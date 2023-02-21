# SMP Client over UART
This sample is mostly a changed version of [SMP Client over BLE](../smp_client_ble).

## Disclaimer
This code/configuration is not thoroughly tested or qualified and should be considered provided “as-is”. Please test it with your application and let me know if you find any issues.

## Patch

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

## Testing
This sample is tested with two nRF52840DK's and NCS v2.2.0.  
You need two nRF52840DK's, one for the SMP Client and one for the SMP Server.  
Connect the SMP Client and SMP Server together with P1.01 and P1.02. Remember to connect TX-RX.
Connect both NRF52840DK's to a computer using USB cables via the J2 USB header.

### Prepare SMP Server
Open [SMP Server over UART example](../../smp/mcuboot_smp_uart).  
Copy [app.update](./app.update) into the SMP Server over UART example.  
Build and flash the SMP Server over UART example:  
```
west build -p -b nrf52840dk_nrf52840
west flash --erase
```  
Change something in [main.c](../../smp/mcuboot_smp_uart/src/main.c) of the SMP Server example.
Build the SMP Server example again. But do not flash it.
```
west build
```
Copy `mcuboot_smp_uart/build/zephyr/app_update.bin` to `smp_client_uart/app_update.bin`.  

### Build and run the SMP Client
Build and flash the SMP Client sample:
```
west build -p -b nrf52840dk_nrf52840
west flash --erase
```  

From now on, instructions will be for the SMP Client unless otherwise specified.  
Press button 1. If the SMP Client lists images, the connection works.  

### Upload app\_update.bin to SMP Client
To upload `app_update.bin` to the SMP Client, Serial Recovery will be used.  
Hold button 1 while resetting the DK. LED1 should light up to indicate serial recovery mode.  
Upload `app_update.bin` using mcumgr: 
```
mcumgr -c acm0 image list
mcumgr -c acm0 image upload app_update.bin -e -n 2
```
After the upload is complete, reset the DK.

### Upload app\_update.bin from SMP Client to SMP Server
Press button 2 on the SMP Client to initiate the transfer.  
Wait until transfer is complete.  
Press button 1 to list images. You should now see two slots listed.  
Press button 3 to mark image as "Test". A image list is returned. 
The secondary slot should be listed as "pending: true".  
Connect the SMP Server to a serial terminal to see logs.
Reset the SMP Server. The SMP Server should now be updated. Verify by watching logs from serial terminal.

