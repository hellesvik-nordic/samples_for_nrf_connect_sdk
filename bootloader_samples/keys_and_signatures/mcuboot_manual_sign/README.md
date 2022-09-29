# MCUBoot sample using SMP Server and manually signed images
Disclaimer: It is recommended to not do this manually, and instead use our build system and its automatically generated files. See [Using MCUBoot in nRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/mcuboot/readme-ncs.html)

## Preparation of Developement Kits
Disable the Mass Storage feature on the device, so that it does not interfere with mcumgr through the debugger:
```
JLinkExe -device NRF52840_XXAA -if SWD -speed 4000 -autoconnect 1 -SelectEmuBySN SEGGER_ID
J-Link>MSDDisable
Probe configured successfully.
J-Link>exit
```

## Manual signing
First, generate private and public keys using imgtool:
```
mkdir keys
cd keys
$NRF_CONNECT_SDK/bootloader/mcuboot/scripts/imgtool.py keygen -k test_priv.pem -t rsa-2048
$NRF_CONNECT_SDK/bootloader/mcuboot/scripts/imgtool.py getpub -k test_priv.pem > test_pub.c
```

Initially build the sample to generate a build folder. The private key in "child_image/mcuboot/tmp_priv.pem" is not important here.
Copy the test_pub.c into mcuboot. This will make it use your key instead of the one generated from child_image/mcuboot/tmp_priv.pem:
```
cp keys/test_pub.c build/mcuboot/zephyr/autogen-pubkey.c
```

Then build and flash the applicaion using west. This build must not be pristine.
The application you have flashed is signed by a private key not fitting the MCUBoot public key. The application will not start.
Next we will sign and program your application.

Now use imgtool to sign your application, both for hex file and for bin file.
```
$NRF_CONNECT_PATH/bootloader/mcuboot/scripts/imgtool.py sign --header-size 0x200 --align 4 --slot-size 0x79e00 --version 1.0.0 --pad-header --key keys/test_priv.pem build/zephyr/app_to_sign.bin app_manually_signed.bin
$NRF_CONNECT_PATH/bootloader/mcuboot/scripts/imgtool.py sign --header-size 0x200 --align 4 --slot-size 0x79e00 --version 1.0.0 --pad-header --key keys/test_priv.pem build/zephyr/zephyr.hex app_manually_signed.hex
```
DISCLAIMER: You should make sure the arguments for this signing command are right for your application. See the [imgtool.py documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/imgtool.html).

First time, you need to use nrfjprog to program your application. You can not use mcumgr, as the application must run to have SMP Server active. 
An alternative could be to use serial recovery mode instead.
Program the signed hex using nrfjprog:
```
nrfjprog --sectorerase --reset --program app_manually_signed.hex
```

## Use mcumgr to program new image
Program the new image using [mcumgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/zephyr/guides/device_mgmt/mcumgr.html):
```
mcumgr conn add acm0 type="serial" connstring="dev=/dev/ttyACM0,baud=115200,mtu=512"
mcumgr -c acm0 image list
mcumgr -c acm0 image upload build/zephyr/app_update.bin
mcumgr -c acm0 image list
```
