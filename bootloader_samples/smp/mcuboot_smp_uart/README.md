# MCUBoot SMP simple sample

## Prepare the Debelopement Kit
Disable the Mass Storage feature on the Interface MCU, so that it does not interfere:
```
$ JLinkExe 
J-Link>MSDDisable
Probe configured successfully.
J-Link>SetHWFC Force
New configuration applies immediately.
J-Link>exit
```

## Build and Flash

```
west build -b <board_name>
west flash --recover
```

## Run sample

Change the print in src/main.c to see change.
Rebuild:
```
west build
```

Program the new image using [mcumgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/zephyr/guides/device_mgmt/mcumgr.html):
Find which serial connection the Developement Kit is connected to. This sample assumes /dev/ttyACM0.

```
mcumgr conn add acm0 type="serial" connstring="dev=/dev/ttyACM0,baud=115200,mtu=512"
mcumgr -c acm0 image list
mcumgr -c acm0 image upload build/zephyr/app_update.bin
mcumgr -c acm0 image list
```

Then tell MCUBoot to boot from the new slot next reboot using one of these commands:
```
mcumgr -c acm0 image test <SLOT1_HASH>
```
or
```
mcumgr -c acm0 image confirm <SLOT1_HASH>
```

Lastly, reset the Developement Kit:
```
nrfjprog --reset
```

## Extra steps for the nRF5340

To update the Network Core, follow the guide above, but for change the "upload" step with the following:
```
mcumgr -c acm1 image upload -n 3 build/zephyr/net_core_app_update.bin
```
Wait 2 minutes for the image to be copied to the netcore, and reset the nRF5340 after.

The -n numbers are set as follows:
```
slot 0 - image 0 primary
slot 1 - image 0 secondary
slot 2 - image 1 primary ("Virtual" slot)
slot 3 - image 1 secondary
```
