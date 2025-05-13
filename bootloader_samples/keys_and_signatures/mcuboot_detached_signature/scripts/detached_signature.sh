(

if [ ! -d build ]; then
  cd .. # Temporarily go to the project folder
  if [ ! -d build ]; then
    echo "No build folder found. Remember to build first."
    exit 1
  fi
fi

alias imgtool="${HOME}/nrf_connect_sdk/bootloader/mcuboot/scripts/imgtool.py"

KEYS_PATH="keys_and_data/keys"
DATA_PATH="keys_and_data/data"

PROJECT_NAME="$(basename "$PWD")"
DEBUGGER_SNR=683304854

APP_HEX="build/$PROJECT_NAME/zephyr/zephyr.hex"
APP_BIN="build/$PROJECT_NAME/zephyr/zephyr.bin"
MCUBOOT_HEX="build/mcuboot/zephyr/zephyr.hex"
APP_SIGNED_HEX=$DATA_PATH/zephyr.signed.hex
APP_SIGNED_BIN=$DATA_PATH/zephyr.signed.bin


build_clean() {
  echo "------------build_clean--------------------"
  rm -r build/
  west build -p -b nrf52840dk/nrf52840
}

flash_clean() {
  echo "------------flash_clean--------------------"
  nrfutil device erase
  nrfutil device recover
  nrfutil device program --serial-number $DEBUGGER_SNR --firmware $MCUBOOT_HEX
  nrfutil device program --serial-number $DEBUGGER_SNR --firmware $APP_SIGNED_HEX --options chip_erase_mode=ERASE_RANGES_TOUCHED_BY_FIRMWARE
  nrfutil device reset
}

BLACK_BOX_PATH="scripts/black_box"
black_box_init() {
  echo "------------black_box_init--------------------"
  source ${BLACK_BOX_PATH}/init.sh # Will generate a public key inside the keys dir
}

black_box_sign_digest() {
  echo "------------black_box_sign_digest--------------------"
  source ${BLACK_BOX_PATH}/sign_digest.sh # Will generate a signature inside the data dir
}

black_box_sign_payload() {
  echo "------------black_box_sign_digest--------------------"
  source ${BLACK_BOX_PATH}/sign_payload.sh # Will generate a signature inside the data dir
}

write_public_key_to_build() {
  echo "------------write_public_key_to_build--------------------"
  imgtool getpub -k $KEYS_PATH/public_key.pem > $KEYS_PATH/pub.c # Convert .pem to .c for MCUboot
  cp $KEYS_PATH/pub.c build/mcuboot/zephyr/autogen-pubkey.c
  west build # Rebuild to compile in the new key
}


VERSION="1.0.0"
HEADER_SIZE=0x200
MCUBOOT_PRIMARY_SIZE=0x7a000 # From device partitioning. 
ALIGN=4

DIGEST=${DATA_PATH}/digest.bin
generate_app_digest() {
  echo "------------generate_app_digest--------------------"
  imgtool sign -v ${VERSION} -H ${HEADER_SIZE} -S ${MCUBOOT_PRIMARY_SIZE} --align ${ALIGN} --pad-header --vector-to-sign digest ${APP_HEX} ${DIGEST}
}

PAYLOAD=${DATA_PATH}/payload.bin
generate_app_payload() {
  echo "------------generate_app_payload--------------------"
  imgtool sign -v ${VERSION} -H ${HEADER_SIZE} -S ${MCUBOOT_PRIMARY_SIZE} --align ${ALIGN} --pad-header --vector-to-sign payload ${APP_HEX} ${PAYLOAD}
}

sign_hex_with_signature() {
  echo "------------sign_hex_with_signature--------------------"
  openssl base64 -in ${DATA_PATH}/signature.sig -out ${DATA_PATH}/signature.b64
  imgtool sign -v ${VERSION} -H ${HEADER_SIZE} -S ${MCUBOOT_PRIMARY_SIZE} --align ${ALIGN} --pad-header --fix-sig-pubkey ${KEYS_PATH}/public_key.pem --fix-sig ${DATA_PATH}/signature.b64 ${APP_HEX} ${APP_SIGNED_HEX}
}

sign_bin_with_signature() {
  echo "------------sign_hex_with_signature--------------------"
  openssl base64 -in ${DATA_PATH}/signature.sig -out ${DATA_PATH}/signature.b64
  imgtool sign -v ${VERSION} -H ${HEADER_SIZE} -S ${MCUBOOT_PRIMARY_SIZE} --align ${ALIGN} --pad-header --fix-sig-pubkey ${KEYS_PATH}/public_key.pem --fix-sig ${DATA_PATH}/signature.b64 ${APP_BIN} ${APP_SIGNED_BIN}
}


# Debug functions below --------------------

cheat_sign_image(){
  APP_SIGNED_HEX=$DATA_PATH/cheat.signed.hex
  APP_SIGNED_BIN=$DATA_PATH/cheat.signed.bin
  echo "------------manually_generate_signed_hex_for_programming--------------------"
  # Only for testing. If we do not have access to the private key, this method can not be used.
  # imgtool sign [options] INFILE OUTFILE
  imgtool sign -v ${VERSION} -H ${HEADER_SIZE} --pad-header -S ${MCUBOOT_PRIMARY_SIZE} --align ${ALIGN} -k ${BLACK_BOX_PATH}/private_key.pem ${APP_HEX} ${APP_SIGNED_HEX}
  imgtool sign -v ${VERSION} -H ${HEADER_SIZE} --pad-header -S ${MCUBOOT_PRIMARY_SIZE} --align ${ALIGN} -k ${BLACK_BOX_PATH}/private_key.pem ${APP_BIN} ${APP_SIGNED_BIN}
}

cheat_flash() {
  APP_SIGNED_HEX=$DATA_PATH/cheat.signed.hex
  APP_SIGNED_BIN=$DATA_PATH/cheat.signed.bin
  echo "------------flash_clean--------------------"
  nrfutil device erase
  nrfutil device recover
  nrfutil device program --serial-number $DEBUGGER_SNR --firmware $MCUBOOT_HEX
  nrfutil device program --serial-number $DEBUGGER_SNR --firmware $APP_SIGNED_HEX --options chip_erase_mode=ERASE_RANGES_TOUCHED_BY_FIRMWARE
  nrfutil device reset
}

dumpinfo(){
  imgtool dumpinfo $DATA_PATH/zephyr.signed.bin > $DATA_PATH/zephyr.signed.dump
  imgtool dumpinfo $DATA_PATH/cheat.signed.bin > $DATA_PATH/cheat.signed.dump
}

# Runs script. Remember to comment out functions when not needed anymore
black_box_init # Generate keys. Use only once
write_public_key_to_build 
generate_app_payload
black_box_sign_payload
sign_hex_with_signature # Only use this on first flash
flash_clean # Only use this on first flash. 
sign_bin_with_signature # Generate zephyr.signed.bin for DFU

# Debugging functions below
# cheat_sign_image
# cheat_flash
# dumpinfo

)

