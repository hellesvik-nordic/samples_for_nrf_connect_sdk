# PSA CSR sample
This sample is uses the [PSA Crypto RSA sample](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/samples/crypto/rsa/README.html) as a base.  
This sample (will soon) show how to do a Certificate Signing Request (CSR) with the PSA Crypto APIs.

## Patching sdk-nrf
To run the sample, you need to apply a patch first:
```
cd NCS_PATH/nrf/
git apply PATH_TO/csr_fixed.patch
```

## Docs
The CSR part of this was done using these docs on [MbedTLS: Using opaque ECDSA keys to generate certificate signing requests (CSRs)](https://os.mbed.com/docs/mbed-os/v6.16/porting/using-psa-enabled-mbed-tls.html#using-opaque-ecdsa-keys-to-generate-certificate-signing-requests-csrs).

