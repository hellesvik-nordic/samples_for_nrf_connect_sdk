# MCUBoot SMP simple sample for nRF5340
Disable the Mass Storage feature on the device, so that it does not interfere:
```
JLinkExe -device NRF5340_XXAA -if SWD -speed 4000 -autoconnect 1 -SelectEmuBySN SEGGER_ID
J-Link>MSDDisable
Probe configured successfully.
J-Link>exit
```

## Build and run the sample
Build and flash the program using west:
```
west build -b nrf5340dk_nrf5340_cpuapp
west flash
```

Change something to see a change.
Build the program again using west. Do not flash this time.

Program the new image using [mcumgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/zephyr/guides/device_mgmt/mcumgr.html):
```
mcumgr conn add acm1 type="serial" connstring="dev=/dev/ttyACM1,baud=115200,mtu=512"
mcumgr -c acm1 image list
```

To update the Application Core:
```
mcumgr -c acm1 image upload build/zephyr/app_update.bin
```


Then tell MCUBoot to boot from the new slot next reboot using one of these commands:
```
mcumgr -c acm1 image test <SLOT1_HASH>
mcumgr -c acm1 image confirm <SLOT1_HASH>
```

Reset the nRF5340 after.

To update the Network Core:

```
mcumgr -c acm1 image upload -n 3 build/zephyr/net_core_app_update.bin
```
Wait 2 minutes for the image to be copied to the netcore, and reset the nRF5340 after.


Then tell MCUBoot to boot from the new slot next reboot using one of these commands:
```
mcumgr -c acm1 image test <SLOT1_HASH>
mcumgr -c acm1 image confirm <SLOT1_HASH>
```

The -n numbers are set as follows:
```
slot 0 - image 0 primary
slot 1 - image 0 secondary
slot 3 - image 1 primary
slot 4 - image 1 secondary (Does not exist)
```

Lastly, reset the DK by pressing the reset button or run:
```
nrfjprog --reset
```
