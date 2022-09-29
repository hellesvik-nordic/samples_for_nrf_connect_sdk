# MCUBoot Custom Key with SMP Server

See the [Simple SMP sample](../mcuboot_smp) for a guide on how to use the SMP Server.

The `child_image/custom_priv.pem` key was generated using:
```
${NCS_PATH}/bootloader/mcuboot/scripts/imgtool.py keygen -k custom_priv.pem -t ecdsa-p256
```
Aslso see the [Imgtool documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/imgtool.html).
