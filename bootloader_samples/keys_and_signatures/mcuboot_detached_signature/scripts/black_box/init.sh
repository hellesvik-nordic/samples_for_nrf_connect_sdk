KEYS_PATH="keys_and_data/keys"
BLACK_BOX_PATH="scripts/black_box"

openssl genpkey -algorithm RSA -out ${BLACK_BOX_PATH}/private_key.pem -pkeyopt rsa_keygen_bits:2048 
openssl rsa -pubout -in ${BLACK_BOX_PATH}/private_key.pem -out ${KEYS_PATH}/public_key.pem 
