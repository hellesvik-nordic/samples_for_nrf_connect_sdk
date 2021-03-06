# MCUBoot SMP simple sample
Disable the Mass Storage feature on the device, so that it does not interfere:
```
JLinkExe -device NRF52840_XXAA -if SWD -speed 4000 -autoconnect 1 -SelectEmuBySN SEGGER_ID
J-Link>MSDDisable
Probe configured successfully.
J-Link>exit
```
Build and flash the program using west.

Change something to see a change.
Build the program again using west. Do not flash this time.

Program the new image using [mcumgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/zephyr/guides/device_mgmt/mcumgr.html):
```
mcumgr conn add acm0 type="serial" connstring="dev=/dev/ttyACM0,baud=115200,mtu=512"
mcumgr -c acm0 image list
mcumgr -c acm image upload build/zephyr/app_update.bin
mcumgr -c acm0 image list
```


See [Using MCUboot in NRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/readme-ncs.html) for more informationon.

The `child_image/custom_priv.pem` key was generated using:
```
${NCS_PATH}/bootloader/mcuboot/scripts/imgtool.py keygen -k custom_priv.pem -t ecdsa-p256
```
Also see the [Imgtool documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/imgtool.html).
