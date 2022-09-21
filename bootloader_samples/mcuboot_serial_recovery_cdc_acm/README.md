# MCUBoot Serial Recovery Sample over USB CDC

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
west build -b nrf52840dk_nrf52840
west flash
```

## Serial Recovery
To activate the Serial Recovery Mode, hold Button 1 while restarting the Developement Kit.
Now, you should be able communicate with the bootloader using [mcumgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/zephyr/guides/device_mgmt/mcumgr.html):
```
mcumgr conn add acm1 type="serial" connstring="dev=/dev/ttyACM1,baud=115200,mtu=512"
mcumgr -c acm1 image list
```
The list command should print:
```
Images:
 image=0 slot=0
    version: 0.0.0.0
    bootable: false
    flags:
    hash: Unavailable
Split status: N/A (0)
```

Change something in the sample to see a difference. Build the sample again and upload the image using mcumgr:
```
west build 
mcumgr -c acm1 image upload build/zephyr/app_update.bin
mcumgr -c acm1 reset
```

After the reset, your new code should run on the nRF52.

## Over CDC_ACM
The serial communication for mcumgr goes through CDC_ACM, so you need to connect an USB to the nRF_USB port.
