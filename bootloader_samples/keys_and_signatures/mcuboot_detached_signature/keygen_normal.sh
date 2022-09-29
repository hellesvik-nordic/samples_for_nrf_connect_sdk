alias imgtool2.py="${HOME}/git/mcuboot_tmp/scripts/imgtool.py"

mkdir -p keys
imgtool2.py keygen -k keys/priv.pem -t rsa-2048
imgtool2.py getpub -k keys/priv.pem > keys/pub.c

west build -p -b nrf52840dk_nrf52840
cp keys/pub.c build/mcuboot/zephyr/autogen-pubkey.c
west flash

openssl rsa -in keys/priv.pem -pubout -out keys/pub.pem -outform PEM -RSAPublicKey_out
imgtool2.py sign -v 1.0.0 -H 0x200 -S 0x79e00 --align 8  --pad-header --key keys/priv.pem  build/zephyr/zephyr.hex zephyr_signed.hex
sleep 2
nrfjprog --sectorerase --program zephyr_signed.hex --reset

