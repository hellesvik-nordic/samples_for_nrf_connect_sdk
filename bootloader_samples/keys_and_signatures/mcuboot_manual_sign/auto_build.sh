#!/bin/bash
NRF_CONNECT_SDK=$HOME/ncs
(cd keys
$NRF_CONNECT_SDK/bootloader/mcuboot/scripts/imgtool.py getpub -k test_priv.pem > test_pub.c
)
west build -p -b nrf52840dk_nrf52840
cp keys/test_pub.c build/mcuboot/zephyr/autogen-pubkey.c

$NRF_CONNECT_SDK/bootloader/mcuboot/scripts/imgtool.py sign --header-size 0x200 --align 4 --slot-size 0x79e00 --version 1.0.0 --pad-header --key keys/test_priv.pem build/zephyr/app_to_sign.bin app_manually_signed.bin
$NRF_CONNECT_SDK/bootloader/mcuboot/scripts/imgtool.py sign --header-size 0x200 --align 4 --slot-size 0x79e00 --version 1.0.0 --pad-header --key keys/test_priv.pem build/zephyr/zephyr.hex app_manually_signed.hex

