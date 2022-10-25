# MCUBoot Custom Key with SMP Server

See the [Simple SMP sample](../mcuboot_smp) for a guide on how to use the SMP Server.

The `child_image/custom_priv.pem` key was generated using:
```
${NCS_PATH}/bootloader/mcuboot/scripts/imgtool.py keygen -k custom_priv.pem -t ecdsa-p256
```
Also see the [Imgtool documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/imgtool.html).


## Test that it fails
After testing this once successfully, try to create a new key as described above.  
Then build the project with the new key(not flash).  
Perform DFU with the new app\_update.bin.  
Now the DFU should fail, since you have used the wrong key.
