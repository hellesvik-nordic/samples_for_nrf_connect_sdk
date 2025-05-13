KEYS_PATH="keys_and_data/keys"
DATA_PATH="keys_and_data/data"
BLACK_BOX_PATH="scripts/black_box"

openssl dgst -sha256 -sign ${BLACK_BOX_PATH}/private_key.pem -out ${DATA_PATH}/signature.sig ${DATA_PATH}/digest.bin
