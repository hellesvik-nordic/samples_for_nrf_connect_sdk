# MCUBoot Simple Sample
Simplest possible(I hope) sample for using NSIB.
Build and flash the program using west.

Build the program again using west. Do not flash this time.

Program the new image using:
```
nrfjprog --reset --sectorerase --program build/zephyr/app_signed.hex
```

See [Using MCUboot in NRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/readme-ncs.html) for more informationon.

The `child_image/custom_priv.pem` key was generated using:
```
${NCS_PATH}/bootloader/mcuboot/scripts/imgtool.py keygen -k custom_priv.pem -t ecdsa-p256
```
Also see the [Imgtool documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/imgtool.html).
