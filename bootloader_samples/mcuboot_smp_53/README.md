# MCUBoot SMP simple sample for nRF5340
Disable the Mass Storage feature on the device, so that it does not interfere:
```
JLinkExe -device NRF5340_XXAA -if SWD -speed 4000 -autoconnect 1 -SelectEmuBySN SEGGER_ID
J-Link>MSDDisable
Probe configured successfully.
J-Link>exit
```
Build and flash the program using west.

Change something to see a change.
Build the program again using west. Do not flash this time.

Program the new image using [mcumgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/zephyr/guides/device_mgmt/mcumgr.html):
```
mcumgr conn add acm1 type="serial" connstring="dev=/dev/ttyACM1,baud=115200,mtu=512"
mcumgr -c acm1 image list
mcumgr -c acm1 image upload build/zephyr/app_update.bin
mcumgr -c acm1 image list
```

Then tell MCUBoot to boot from the new slot next reboot using one of these commands:
```
mcumgr -c acm1 image test <SLOT1_HASH>
mcumgr -c acm1 image confirm <SLOT1_HASH>
```

Lastly, reset the DK by pressing the reset button or running one of these commands:
```
mcumgr -c acm1 reset
nrfjprog --reset
```



See [Using MCUboot in NRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/readme-ncs.html) for more informationon.

The `child_image/custom_priv.pem` key was generated using:
```
${NCS_PATH}/bootloader/mcuboot/scripts/imgtool.py keygen -k custom_priv.pem -t ecdsa-p256
```
Aslso see the [Imgtool documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/imgtool.html).
