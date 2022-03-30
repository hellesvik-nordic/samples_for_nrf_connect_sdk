alias imgtool2.py="${HOME}/git/mcuboot_tmp/scripts/imgtool.py"

mkdir -p keys
imgtool2.py keygen -k keys/priv.pem -t rsa-2048
imgtool2.py getpub -k keys/priv.pem > keys/pub.c

west build -p -b nrf52840dk_nrf52840
cp keys/pub.c build/mcuboot/zephyr/autogen-pubkey.c
west flash

openssl rsa -in keys/priv.pem -pubout -out keys/pub.pem -outform PEM -RSAPublicKey_out
imgtool2.py sign -v 1.0.0 -H 0x200 -S 0x79e00 --align 8  --fix-sig-pubkey keys/pub.pem --vector-to-sign payload  --pad-header build/zephyr/zephyr.hex keys/data_to_sign.bin
openssl dgst -sha256  -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:32 -sigopt rsa_mgf1_md:sha256 -sign keys/priv.pem -out keys/openssl_sig.sig keys/data_to_sign.bin
openssl base64 -in keys/openssl_sig.sig -out keys/openssl_sig_base64.sig
imgtool2.py sign -v 1.0.0  -H 0x200 -S 0x79e00 --align 8 --fix-sig keys/openssl_sig_base64.sig --fix-sig-pubkey keys/pub.pem --pad-header  build/zephyr/zephyr.hex signed_app_image.hex
sleep 2
nrfjprog --sectorerase --program signed_app_image.hex --reset

