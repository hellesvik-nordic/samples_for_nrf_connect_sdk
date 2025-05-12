(

if [ ! -d build ]; then
  cd .. # Temporarily go to the project folder
  if [ ! -d build ]; then
    echo "No build folder found. Remember to build first."
    exit 1
  fi
fi

alias imgtool="${HOME}/nrf_connect_sdk/bootloader/mcuboot/scripts/imgtool.py"


PROJECT_NAME="$(basename "$PWD")"
DEBUGGER_SNR=682244907


APP_HEX="build/$PROJECT_NAME/zephyr/zephyr.hex"
MCUBOOT_HEX="build/mcuboot/zephyr/zephyr.hex"
APP_SIGNED_HEX=$DATA_PATH/zephyr.signed.hex
APP_SIGNED_BIN=$DATA_PATH/zephyr.signed.bin


build_clean() {
  echo "------------build_clean--------------------"
  rm -r build/
  west build -p -b nrf52dk/nrf52832
}

flash_clean() {
  echo "------------flash_clean--------------------"
  nrfutil device erase
  nrfutil device recover
  nrfutil device program --serial-number $DEBUGGER_SNR --firmware $MCUBOOT_HEX
  nrfutil device program --serial-number $DEBUGGER_SNR --firmware $APP_SIGNED_HEX --options chip_erase_mode=ERASE_RANGES_TOUCHED_BY_FIRMWARE
  nrfutil device reset
}

generate_private_key() {
  echo "------------generate_private_key--------------------"
  imgtool keygen -k $KEYS_PATH/priv.pem -t rsa-2048
}

write_public_key_to_build() {
  echo "------------write_public_key_to_build--------------------"
  imgtool getpub -k $KEYS_PATH/priv.pem > $KEYS_PATH/pub.c
  cp $KEYS_PATH/pub.c build/mcuboot/zephyr/autogen-pubkey.c
}

VERSION="1.0.0"
HEADER_SIZE=0x200
MCUBOOT_PRIMARY_SIZE=0x39e00 # From device partitioning. 
ALIGN=8

attatched_sig_manually_generate_signed_hex_for_programming(){
  echo "------------manually_generate_signed_hex_for_programming--------------------"
  #imgtool sign [options] INFILE OUTFILE
  imgtool sign -v $VERSION -H $HEADER_SIZE -S $MCUBOOT_PRIMARY_SIZE --align $ALIGN --pad-header -k $KEYS_PATH/priv.pem $APP_HEX $APP_SIGNED_HEX
  imgtool sign -v $VERSION -H $HEADER_SIZE -S $MCUBOOT_PRIMARY_SIZE --align $ALIGN --pad-header -k $KEYS_PATH/priv.pem $APP_HEX $APP_SIGNED_BIN
}

get_

# build_clean
generate_private_key
write_public_key_to_build
attatched_sig_manually_generate_signed_hex_for_programming
flash_clean


# imgtool2.py sign -v $VERSION -H $HEADER_SIZE -S $MCUBOOT_PRIMARY_SIZE --align $ALIGN  --vector-to-sign payload  --pad-header $APP_HEX $DATA_PATH/data_to_sign.bin
)

