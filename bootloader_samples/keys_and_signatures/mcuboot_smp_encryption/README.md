# MCUboot with encryption
The nRF Connect SDK does not have official support for encrypted DFU.  
However, upastream MCUboot have built-in support for this, so we can leverage this.  
Please read the disclaimer.

## Disclaimer
This code/configuration is not thoroughly tested or qualified and should be considered provided “as-is”. Please test it with your application and let me know if you find any issues.  
This sample uses symmetric encryption. If someone breaks into a device and steals the key, they can decrypted firmware updates.  
Encryption does not replace signing of images, and this sample uses default mcuboot keys for signing. See [MCUBoot Custom Key with SMP Server](../mcuboot_smp_custom_key) for how a custom signing key can be added.

## Docs
See [MCUboot: Encryped images](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/v2.4.0/mcuboot/encrypted_images.html) for an explanation on how MCUboot handles encryption.

## Patch the nRF repo
The nRF Connect SDK generates [Build files](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/v2.4.0/nrf/config_and_build/config_and_build_system.html#output-build-files) so that developers have an easier time doing DFU. [nrf.patch](./nrf.patch) changes the nRF Connect SDK so that app\_update.bin is automatically encrypted (when encryption is enabled).
To apply the patch, do this in a terminal or git bash:
```
cd <PATH_TO>/ncs/nrf
git apply <PATH_TO>/nrf.patch
```

## Key generation
The `custom_key_dir/encryption_key.pem` key was generated using:
```
${NCS_PATH}/bootloader/mcuboot/scripts/imgtool.py keygen -k custom_key_dir/encryption_key.pem -t ecdsa-p256
```

## Run sample
Due to the patch above, app\_update.bin is automatically encrypted.  
See steps in the [MCUBoot SMP simple sample](../../smp/mcuboot_smp_uart) for how to test this sample.  
To see imgtool command for encrypting images: 
```
west -vvv build ...
```

