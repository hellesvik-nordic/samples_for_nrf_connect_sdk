# PSA CSR sample
This sample is uses the [PSA Crypto RSA sample](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/samples/crypto/rsa/README.html) as a base.  
This sample (will soon) show how to do CSR with the PSA Crypto APIs.

## Patching sdk-nrf
To run the sample, you need to apply a patch first:
```
cd NCS_PATH/nrf/
git apply PATH_TO/csr_fixed.patch
```
