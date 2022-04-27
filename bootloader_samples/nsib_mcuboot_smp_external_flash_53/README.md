# MCUBoot SMP with external flash for the nRF5340
This sample has been tested for the nRF5340DK.

Disable the Mass Storage feature on the device, so that it does not interfere.
SEGGER_ID is the identification printed on the sticker on the Developement Kit.
```
JLinkExe -device NRF5340_XXAA -if SWD -speed 4000 -autoconnect 1 -SelectEmuBySN SEGGER_ID
J-Link>MSDDisable
Probe configured successfully.
J-Link>exit
```

Build and flash the program using west.
```
west build -p -b nrf5340dk_nrf5340_cpuapp
west flash --recover
```

Change something to see a change.
Build the program again using west. Do not flash this time.
```
west build
```


Program the new image using [mcumgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/zephyr/guides/device_mgmt/mcumgr.html):
```
mcumgr conn add acm0 type="serial" connstring="dev=/dev/ttyACM0,baud=115200,mtu=512"
mcumgr -c acm0 image list
mcumgr -c acm image upload -n 3 build/zephyr/net_core_app_update.bin
#Wait for 2 minutes to allow for b0n to move the image to the net core.
mcumgr -c acm image upload build/zephyr/app_update.bin
mcumgr -c acm0 image list
```
