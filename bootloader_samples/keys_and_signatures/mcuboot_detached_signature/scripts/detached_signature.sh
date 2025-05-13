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
  source ${BLACK_BOX_PATH}/init.sh # Will generate a public key for us inside our keys dir.
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

attatched_sig_manually_generate_signed_hex_for_programming(){
  echo "------------manually_generate_signed_hex_for_programming--------------------"
  # Only for testing. If we do not have access to the private key, this method can not be used.
  # imgtool sign [options] INFILE OUTFILE
  imgtool sign -v ${VERSION} -H ${HEADER_SIZE} -S ${MCUBOOT_PRIMARY_SIZE} --align ${ALIGN} --pad-header -k ${KEYS_PATH}/priv.pem ${APP_HEX} ${APP_SIGNED_HEX}
}

# build_clean
black_box_init
sleep 1
write_public_key_to_build
# attatched_sig_manually_generate_signed_hex_for_programming
# sleep 2
# flash_clean


# imgtool2.py sign -v $VERSION -H $HEADER_SIZE -S $MCUBOOT_PRIMARY_SIZE --align $ALIGN  --vector-to-sign payload  --pad-header $APP_HEX $DATA_PATH/data_to_sign.bin
)

