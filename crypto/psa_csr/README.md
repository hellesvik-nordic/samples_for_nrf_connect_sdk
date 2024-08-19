# PSA CSR sample
This sample (will soon) show how to do a Certificate Signing Request (CSR) with the PSA Crypto APIs.

## Patching sdk-nrf
To run the sample, you need to apply a patch first:
```
cd NCS_PATH/nrf/
git apply PATH_TO/csr_fixed_nrf.patch
cd NCS_PATH/modules/crypto/mbedtls/
git apply PATH_TO/csr_fixed_mbedtls.patch
```

## Docs
This sample is uses the [PSA Crypto RSA sample](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/samples/crypto/rsa/README.html) as a base.  
This sample CSR code is from the [Zephyr TF-M PSA Crypto CSR sample](https://docs.nordicsemi.com/bundle/ncs-latest/page/zephyr/samples/tfm_integration/psa_crypto/README.html).  
The Zephyr sample from above uses Legact MbedTLS drivers for CSR. The converting of this sample to use PSA Crypto API directly for CSR was done using these docs on [MbedTLS: Using opaque ECDSA keys to generate certificate signing requests (CSRs)](https://os.mbed.com/docs/mbed-os/v6.16/porting/using-psa-enabled-mbed-tls.html#using-opaque-ecdsa-keys-to-generate-certificate-signing-requests-csrs).  

## Disclaimer
When writing this sample, I did not verify the output. As a user of this sample, you should test the output to make sure it does what you expect.
