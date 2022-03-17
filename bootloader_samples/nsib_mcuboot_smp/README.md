# Sample on how to use NSIB(b0) to update MCUBoot feat. SMP Server
## Preparation of the Developement Kit
Disable the Mass Storage feature on the device, so that it does not interfere:
```
JLinkExe -device NRF52840_XXAA -if SWD -speed 4000 -autoconnect 1 -SelectEmuBySN SEGGER_ID
J-Link>MSDDisable
Probe configured successfully.
J-Link>exit
```

## Build 
Build and flash the program using west.

Now increment CONFIG_FW_INFO_FIRMWARE_VERSION in child_image/mcuboot/prj.conf.

To make the change visible, make a change to MCUBoot, for example add a log in $NRF_CONNECT_SDK/bootloader/mcuboot/boot/zephyr/main.c. 

Build again, and upload the new image using mcumgr:
```
mcumgr conn add acm0 type="serial" connstring="dev=/dev/ttyACM0,baud=115200,mtu=512"
mcumgr -c acm0 image list
mcumgr -c acm0 image upload build/zephyr/signed_by_mcuboot_and_b0_s1_image_update.bin
mcumgr -c acm0 image list
```

Copy the hash of the newly uploaded image, and use it to confirm it, making the new image run at next reboot, such as:
```
mcumgr -c acm0 image confirm 2348de4f84cb19c1c2721662ad1275da5c21eca749da9b32db20d2c9dffb47c0
```

Then reboot the Developement Kit. This will load the new MCUBoot image its slot. 
Reboot the Developement Kit again to load using the new version of MCUBoot.

Bug: 
After updating MCUBoot, I were not able to update it again. 

## Key generation
The keys in this sample was generated using:
```
${NCS_PATH}/bootloader/mcuboot/scripts/imgtool.py keygen -k custom_priv.pem -t ecdsa-p256
```
Also see the [Imgtool documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/imgtool.html).
