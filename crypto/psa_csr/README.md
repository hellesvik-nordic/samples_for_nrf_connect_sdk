# PSA CSR sample
This sample (will soon) show how to do a Certificate Signing Request (CSR) with the PSA Crypto APIs.

# TF-M
This sample was tested without Trusted Firmware-M. It gets some build errors with, but I beleive this should be mostly due to different default configuration with TF-M.

## Patching sdk-nrf
To run the sample, you need to apply a patch first:
```
cd NCS_PATH/nrf/
git apply PATH_TO/csr_fixed_nrf.patch
```

## Docs
This sample is uses the [PSA Crypto ECDSA sample](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/samples/crypto/ecdsa/README.html) as a base.  
This sample CSR code is from the [Zephyr TF-M PSA Crypto CSR sample](https://docs.nordicsemi.com/bundle/ncs-latest/page/zephyr/samples/tfm_integration/psa_crypto/README.html).  
The Zephyr sample from above uses Legact MbedTLS drivers for CSR. The converting of this sample to use PSA Crypto API directly for CSR was done using these docs on [MbedTLS: Using opaque ECDSA keys to generate certificate signing requests (CSRs)](https://os.mbed.com/docs/mbed-os/v6.16/porting/using-psa-enabled-mbed-tls.html#using-opaque-ecdsa-keys-to-generate-certificate-signing-requests-csrs).  

## Disclaimer
When writing this sample, I did not verify the output. As a user of this sample, you should test the output to make sure it does what you expect.

## Output
Here is the output I get from the sample over logging:
```
*** Booting nRF Connect SDK v2.7.0-5cb85570ca43 ***
*** Using Zephyr OS v3.6.99-100befc70c74 ***
[00:00:00.389,190] <inf> ecdsa: Starting ECDSA example...
[00:00:00.395,263] <inf> ecdsa: Generating random ECDSA keypair...
[00:00:00.426,574] <inf> ecdsa: Signing a message using ECDSA...
[00:00:00.463,195] <inf> ecdsa: Message signed successfully!
[00:00:00.469,512] <inf> ecdsa: ---- Plaintext (len: 100): ----
[00:00:00.476,135] <inf> ecdsa: Content:
                                45 78 61 6d 70 6c 65 20  73 74 72 69 6e 67 20 74 |Example  string t
                                6f 20 64 65 6d 6f 6e 73  74 72 61 74 65 20 62 61 |o demons trate ba
                                73 69 63 20 75 73 61 67  65 20 6f 66 20 45 43 44 |sic usag e of ECD
                                53 41 2e 00 00 00 00 00  00 00 00 00 00 00 00 00 |SA...... ........
                                00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 |........ ........
                                00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 |........ ........
                                00 00 00 00                                      |....             
[00:00:00.561,828] <inf> ecdsa: ---- Plaintext end  ----
[00:00:00.567,779] <inf> ecdsa: ---- SHA256 hash (len: 32): ----
[00:00:00.574,462] <inf> ecdsa: Content:
                                6d bf 34 71 f1 7f cd 8d  99 13 10 e7 95 48 84 60 |m.4q.... .....H.`
                                d3 35 65 8a 82 b6 76 07  5c 3b 79 3b be d7 6e 4f |.5e...v. \;y;..nO
[00:00:00.597,900] <inf> ecdsa: ---- SHA256 hash end  ----
[00:00:00.604,034] <inf> ecdsa: ---- Signature (len: 64): ----
[00:00:00.610,534] <inf> ecdsa: Content:
                                4f 48 15 67 18 7c ea 55  59 84 fe c4 9b 4d 82 05 |OH.g.|.U Y....M..
                                69 33 d0 8d 7d 11 38 f6  62 3d 8c 5d 1f 5c 80 89 |i3..}.8. b=.].\..
                                d2 e5 c1 4e a1 97 a6 2b  ae c1 4d 24 b1 ae de 95 |...N...+ ..M$....
                                a2 fc 65 03 c8 8f 2b e6  2d aa 28 cb 50 55 2a d9 |..e...+. -.(.PU*.
[00:00:00.652,862] <inf> ecdsa: ---- Signature end  ----
[00:00:00.658,843] <inf> ecdsa: Verifying ECDSA signature...
[00:00:00.720,153] <inf> ecdsa: Signature verification was successful!
[00:00:00.727,355] <inf> ecdsa: Setting up CSR...
[00:00:00.732,910] <inf> ecdsa: Adding EC key to PK container completed
[00:00:00.740,295] <inf> ecdsa: Create device Certificate Signing Request
[00:00:00.803,619] <inf> ecdsa: Create device Certificate Signing Request completed
[00:00:00.812,011] <inf> ecdsa: Certificate Signing Request:

-----BEGIN CERTIFICATE REQUEST-----
MIHpMIGQAgEAMC4xDzANBgNVBAoMBkxpbmFybzEbMBkGA1UEAwwSRGV2aWNlIENl
cnRpZmljYXRlMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEEApJRuGVNaKCQRVl
CzOBQsHw+jESD7tAD8qaxybq/qNKJtkX7SXvrmANY5fdnx4BcqGz1uV69/0Nabh9
WV+4XaAAMAoGCCqGSM49BAMCA0gAMEUCIQDTnWbeD0aLvWnHToJUB5/sEuXp0mED
rs89Yu6f97sITwIgGo8YZuKNmGrCnP30jxdGSl2OqFFOtt3GswcAM3HeoJY=
-----END CERTIFICATE REQUEST-----

[00:00:00.853,210] <inf> ecdsa: Encoding CSR as json
[00:00:00.859,405] <inf> ecdsa: Encoding CSR as json completed
[00:00:00.865,905] <inf> ecdsa: Certificate Signing Request in JSON:

{"CSR":"-----BEGIN CERTIFICATE REQUEST-----\nMIHpMIGQAgEAMC4xDzANBgNVBAoMBkxpbmFybzEbMBkGA1UEAwwSRGV2aWNlIENl\ncnRpZmljYXRlMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEEApJRuGVNaKCQRVl\nCzOBQsHw+jESD7tAD8qaxybq/qNKJtkX7SXvrmANY5fdnx4BcqGz1uV69/0Nabh9\nWV+4XaAAMAoGCCqGSM49BAMCA0gAMEUCIQDTnWbeD0aLvWnHToJUB5/sEuXp0mED\nrs89Yu6f97sITwIgGo8YZuKNmGrCnP30jxdGSl2OqFFOtt3GswcAM3HeoJY=\n-----END CERTIFICATE REQUEST-----\n"}
[00:00:00.910,247] <inf> ecdsa: Example finished successfully!
```
