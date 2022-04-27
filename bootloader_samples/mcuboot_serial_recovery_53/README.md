# Serial Recovery for the nRF5340
This sample showcase how to use Serial Recovery over UART to do a Device Firmware Update (DFU) using MCUBoot.


## Prepare the Developement Kit
Disable the Mass Storage feature on the device, so that it does not interfere:
```
JLinkExe -device NRF52840_XXAA -if SWD -speed 4000 -autoconnect 1 -SelectEmuBySN SEGGER_ID
J-Link>MSDDisable
Probe configured successfully.
J-Link>exit
```

## Run the sample
Build and flash the program using west:
```
west build -b nrf5340dk_nrf5340_cpuapp
west flash
```

## Serial Recovery
To activate the Serial Recovery Mode, hold Button 1 while restarting the Developement Kit.
Now, you should be able communicate with the bootloader using [mcumgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/zephyr/guides/device_mgmt/mcumgr.html):
```
mcumgr conn add acm0 type="serial" connstring="dev=/dev/ttyACM0,baud=115200,mtu=512"
mcumgr -c acm0 image list
```
The list command should print:
```
 image=0 slot=0
    version: 0.0.0.0
    bootable: false
    flags:
    hash: Unavailable
 image=1 slot=0
    version: 0.0.0.0
    bootable: false
    flags:
    hash: Unavailable
Split status: N/A (0)

```

Change something in the sample to see a difference. Build the sample again and upload the image using mcumgr. 
```
west build 
```

To update the Application Core:
```
mcumgr -c acm image upload build/zephyr/app_update.bin
```
Reset the nRF5340 after.

To update the Network Core:

```
mcumgr -c acm image upload -n 3 build/zephyr/net_core_app_update.bin
```
Wait 2 minutes for the image to be copied to the netcore, and reset the nRF5340 after.
