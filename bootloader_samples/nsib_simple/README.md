# NSIB Simple sample
Simplest possible(I hope) sample for using Nordic Secure Immutable Bootloader (NSIB).
Build and flash the program using west.

Then increment CONFIG_FW_INFO_FIRMWARE_VERSION.
Build the program again using west. Do not flash this time.

Program the new image to the s0 slot by using:
```
nrfjprog --reset --sectorerase --program build/zephyr/signed_by_b0_s0_image.hex
```

See [Firmware Updates](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/nrf/ug_fw_update.html) for more informationon this.

The `nsib_priv.pem` key was generated using:
```
${NCS_PATH}/bootloader/mcuboot/scripts/imgtool.py keygen -k nsib_priv.pem -t ecdsa-p256
```
Also see the [Imgtool documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/imgtool.html).
