# PSA CSR sample
This sample (will soon) show how to do a Certificate Signing Request (CSR) with the PSA Crypto APIs.

# TF-M
This sample was tested without Trusted Firmware-M. It gets some build errors with, but I beleive this should be mostly due to different default configuration with TF-M.

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

## Output
Here is the output I get from the sample over logging:
```
*** Using Zephyr OS v3.6.99-100befc70c74 ***
[00:00:00.435,119] <inf> rsa: Starting the RSA example...
[00:00:00.440,917] <inf> rsa: Importing RSA keypair...
[00:00:00.446,960] <inf> rsa: RSA private key imported successfully!
[00:00:00.453,796] <inf> rsa: RSA public key imported successfully!
[00:00:00.460,418] <inf> rsa: Signing a message using RSA...
[00:00:04.293,182] <inf> rsa: Signing was successful!
[00:00:04.298,583] <inf> rsa: ---- Plaintext (len: 100): ----
[00:00:04.304,718] <inf> rsa: Content:
                              45 78 61 6d 70 6c 65 20  73 74 72 69 6e 67 20 74 |Example  string t
                              6f 20 64 65 6d 6f 6e 73  74 72 61 74 65 20 62 61 |o demons trate ba
                              73 69 63 20 75 73 61 67  65 20 6f 66 20 52 53 41 |sic usag e of RSA
                              2e 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 |........ ........
                              00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 |........ ........
                              00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 |........ ........
                              00 00 00 00                                      |....             
[00:00:04.368,865] <inf> rsa: ---- Plaintext end  ----
[00:00:04.374,420] <inf> rsa: ---- SHA256 hash (len: 32): ----
[00:00:04.380,645] <inf> rsa: Content:
                              fe 24 65 83 cc 33 44 c9  91 e3 3e b5 c5 b6 62 fe |.$e..3D. ..>...b.
                              52 57 64 a1 b7 a1 6c f2  69 de ca f3 50 5c 31 67 |RWd...l. i...P\1g
[00:00:04.401,947] <inf> rsa: ---- SHA256 hash end  ----
[00:00:04.407,653] <inf> rsa: ---- Signature (len: 512): ----
[00:00:04.413,818] <inf> rsa: Content:
                              07 6c 36 f2 60 ec fa de  51 e1 c5 3c 95 43 c6 59 |.l6.`... Q..<.C.Y
                              73 8d f7 5c a0 dc 38 d3  cd fa 72 04 0c bf be 24 |s..\..8. ..r....$
                              a7 cf a0 91 1e 3e db 6c  49 19 e6 d8 51 d3 18 1a |.....>.l I...Q...
                              4f 13 d9 ae fd 25 41 e4  c9 c1 50 7e eb 42 5d cb |O....%A. ..P~.B].
                              d6 ea 7f 80 00 e8 ea b6  05 2c 40 b5 8a 9c a5 3e |........ .,@....>
                              b2 00 c0 72 52 6a ce c5  1c a5 41 7e 02 d1 b2 60 |...rRj.. ..A~...`
                              57 c7 78 03 3b fe 66 55  86 bb b7 e7 88 00 ea 69 |W.x.;.fU .......i
                              88 9c e0 04 d5 8d 0b 26  8a f4 fa 4f 58 3d a5 52 |.......& ...OX=.R
                              53 d4 6b 3b 9e 0c 53 4f  08 80 67 07 06 5b d3 e0 |S.k;..SO ..g..[..
                              72 c5 cd 9d f0 e7 78 14  89 7b fd bd 1c f0 8c 7d |r.....x. .{.....}
                              57 6d ec 7b 78 43 38 54  84 cc 7e c5 67 98 6a 1f |Wm.{xC8T ..~.g.j.
                              6a ce 2c 46 a2 bc e1 b4  3c 87 57 6c b6 50 04 b1 |j.,F.... <.Wl.P..
                              d6 5e f1 5f 32 60 8e 36  af 43 c1 59 ee 5e aa f3 |.^._2`.6 .C.Y.^..
                              73 a6 6a 98 36 99 f8 f0  17 10 f5 71 bb d3 ff 99 |s.j.6... ...q....
                              9d 09 fd d1 92 0c a7 33  08 bb fb e6 1f e3 6f fb |.......3 ......o.
                              e9 26 fa 0c da 13 5a 99  8b 70 a8 37 05 87 12 4b |.&....Z. .p.7...K
                              4e e4 6b 83 c4 67 e2 16  4c ad 59 db 07 0a 4c e1 |N.k..g.. L.Y...L.
                              8c cf 6c e7 b0 f7 7d 3d  7d e7 94 7a a8 5c 5b 23 |..l...}= }..z.\[#
                              76 91 68 3e fa 36 05 39  12 e6 27 5b 8f a3 dc 7e |v.h>.6.9 ..'[...~
                              ce 11 dc de bf a9 52 11  9f 05 1c ec 85 f1 34 5f |......R. ......4_
                              17 42 3e d5 23 c5 82 2f  49 45 ae f6 60 66 b4 6a |.B>.#../ IE..`f.j
                              89 17 03 bd bd 95 a1 1f  62 09 06 a2 f0 2a 5e d6 |........ b....*^.
                              69 56 cd 8e c6 68 95 27  89 d2 f8 a7 36 7d ac cf |iV...h.' ....6}..
                              f9 c2 ac bc 3c 14 64 a0  c9 78 23 df 15 43 d7 8e |....<.d. .x#..C..
                             fd 69 d5 3a 0e a0 87 88  d0 4f 12 7f 03 ab bd 10 |.i.:.... .O......
                              41 78 30 c6 06 22 87 cf  f5 18 16 16 4c 94 0e 7c |Ax0..".. ....L..|
                              6f 09 e4 7b c5 01 c3 bc  b3 ae 4d 3e 2d 93 4f 83 |o..{.... ..M>-.O.
                              77 36 06 dd 49 49 41 30  93 eb d5 2c 93 71 1d 62 |w6..IIA0 ...,.q.b
                              b2 d3 84 4a 44 06 8c 84  c4 58 0e 93 ed 31 19 78 |...JD... .X...1.x
                              d5 1e ea 9b 0b 62 66 d1  12 f8 ff f0 47 0d 90 6a |.....bf. ....G..j
                              ec 9e e2 9b c6 2d d4 86  b8 b6 1b 29 ab de 39 d2 |.....-.. ...)..9.
                              aa c0 13 c3 06 ad 9d e6  74 e2 81 55 27 bd 6d ff |........ t..U'.m.
[00:00:04.692,138] <inf> rsa: ---- Signature end  ----
[00:00:04.697,692] <inf> rsa: Verifying RSA signature...
[00:00:04.746,002] <inf> rsa: Signature verification was successful!
[00:00:04.752,716] <inf> rsa: Setting up CSR...
[00:00:04.757,873] <inf> rsa: Adding EC key to PK container completed
[00:00:04.764,709] <inf> rsa: Create device Certificate Signing Request
[00:00:08.605,194] <inf> rsa: Create device Certificate Signing Request completed
[00:00:08.613,006] <inf> rsa: Certificate Signing Request:

-----BEGIN CERTIFICATE REQUEST-----
MIIEczCCAlsCAQAwLjEPMA0GA1UECgwGTGluYXJvMRswGQYDVQQDDBJEZXZpY2Ug
Q2VydGlmaWNhdGUwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQC35aZY
aRbac6Esj3GRZ0EC+l1YlQgr3cPQvWxByReX8fQ0yzfcxblXYxb3zmz2idr0qA83
jxpmfB6TXg5jp18bkEghrAIT+TZEUwd3RZzcwQhRt6ZzJjKTspx+ujlnWfEB28b6
TCqsQ0S3a0LmsSD0WsBtIh9kjR21BBBXURKNzV3ZsQbXR4sYXx6W0g2pjvVcgNvF
r+IqIy79Q25mDS1FEqTLRxLC6Lr7mewFV23WoGY6mmNvWJ9y9gpHt+qOrrFSKP3p
FhRGixsuhcZ+mYUiFiMUSSJTHnsEbLl16Nu6lopjOMyfWevPJztmIf5Lo3ZcsGRA
+5MlaLmhxJuAXiXuxqjTnDr+ce9Wqu9KM0fWvnt3m2ZJqlhn77joW4Sgn9zPOGIF
bVuI1RvHOtA601gEoFuLYIIoRL+Jv+G8Dm73mDjUkDih1lpSlUrf8QaUgZVT6LdL
pD8KDr1TOGdkPIYepV23ncibdureOkWVf7C2a8MlMgq9agUjdh2fueHoR7+0D2fr
ZPzdjNoU9VV7zo19sm+rUb9ioZlSkpnK3y8YnimUQuR3PFkEAx/HghOTAbz3Kw/5
RrxEz7cvoZFoJacdfoqYiGOKuu/tkHvAkXlyQfMVjscSXN35IJItfE4BZ86XYfT/
ZErwGvcU+XGXxgeR6b3woDzDe5GwNhA6yWWXFwIDAQABoAAwDQYJKoZIhvcNAQEL
BQADggIBAJohtLPhIpBs7fovhEgW9p3LMv7t9B36lTZw6hV7q9z0LpWlLZfmFwbs
AyDMcON2AEALiwh0ldX50qrsoo7Rd+ljt4BIye4J7zG+TpIO62Zh8UMB0k3bmlNO
0fdi8ZtCjWSwXu+oFr5Kj+DoVPRLmZPHhzrMsvu0mfLibU/G9atIg2+qwurrNIN8
vOIAA/czmhKS/cL1uSXu7tHMeK/HMgUISy7q+2/wWEqez+uTg7Pz1A4ovFEAcvid
c3NiYfwzdVo5gBPxQJosIl9kTLyBy+TLa2YkkvtaBBnjasn/gNnaoGwSGHjzNfFM
REmL0oEMlrT9VbCPslXQBQAA8oABITFKBj3XymiZkby9qS0pDyeIfvFwLuAArD6S
YefhPEhqH2LtjshiVN4BZpB956lHGV9wZ0BrhnhcKDC04hFqvGiFr43gkJrRnM9g
ROOPMaxSQG7iVvqebs9e7VJM+zN0u15TyrrhRIngrMYKVCyQewlrtK9xL86L43LE
uDCoHGlGW7+lCXQ+uThUEdr+3vDZlKVMFp0CUxZce5aQPioyQJxmzWGfXD9iJhwa
bl7HYETIfGdXBprS/HHn89hZJNPf7nFrOroWpPNJKW+YWC/mgGt06ogT6CnHOQrW
aVMG4jSnZD1Hk+fr95RoV5CHkhrbsoiLPf128YpN5cM8cTsRb+Gn
-----END CERTIFICATE REQUEST-----

[00:00:08.761,444] <inf> rsa: Encoding CSR as json
[00:00:08.768,493] <inf> rsa: Encoding CSR as json completed
[00:00:08.774,505] <inf> rsa: Certificate Signing Request in JSON:

{"CSR":"-----BEGIN CERTIFICATE REQUEST-----\nMIIEczCCAlsCAQAwLjEPMA0GA1UECgwGTGluYXJvMRswGQYDVQQDDBJEZXZpY2Ug\nQ2VydGlmaWNhdGUwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQC35aZY\naRbac6Esj3GRZ0EC+l1YlQgr3cPQvWxByReX8fQ0yzfcxblXYxb3zmz2idr0qA83\njxpmfB6TXg5jp18bkEghrAIT+TZEUwd3RZzcwQhRt6ZzJjKTspx+ujlnWfEB28b6\nTCqsQ0S3a0LmsSD0WsBtIh9kjR21BBBXURKNzV3ZsQbXR4sYXx6W0g2pjvVcgNvF\nr+IqIy79Q25mDS1FEqTLRxLC6Lr7mewFV23WoGY6mmNvWJ9y9gpHt+qOrrFSKP3p\nFhRGixsuhcZ+mYUiFiMUSSJTHnsEbLl16Nu6lopjOMyfWevPJztmIf5Lo3ZcsGRA\n+5MlaLmhxJuAXiXuxqjTnDr+ce9Wqu9KM0fWvnt3m2ZJqlhn77joW4Sgn9zPOGIF\nbVuI1RvHOtA601gEoFuLYIIoRL+Jv+G8Dm73mDjUkDih1lpSlUrf8QaUgZVT6LdL\npD8KDr1TOGdkPIYepV23ncibdureOkWVf7C2a8MlMgq9agUjdh2fueHoR7+0D2fr\nZPzdjNoU9VV7zo19sm+rUb9ioZlSkpnK3y8YnimUQuR3PFkEAx/HghOTAbz3Kw/5\nRrxEz7cvoZFoJacdfoqYiGOKuu/tkHvAkXlyQfMVjscSXN35IJItfE4BZ86XYfT/\nZErwGvcU+XGXxgeR6b3woDzDe5GwNhA6yWWXFwIDAQABoAAwDQYJKoZIhvcNAQEL\nBQADggIBAJohtLPhIpBs7fovhEgW9p3LMv7t9B36lTZw6hV7q9z0LpWlLZfmFwbs\nAyDMcON2AEALiwh0ldX50qrsoo7Rd+ljt4BIye4J7zG+TpIO62Zh8UMB0k3bmlNO\n0fdi8ZtCjWSwXu+oFr5Kj+DoVPRLmZPHhzrMsvu0mfLibU/G9atIg2+qwurrNIN8\nvOIAA/czmhKS/cL1uSXu7tHMeK/HMgUISy7q+2/wWEqez+uTg7Pz1A4ovFEAcvid\nc3NiYfwzdVo5gBPxQJosIl9kTLyBy+TLa2YkkvtaBBnjasn/gNnaoGwSGHjzNfFM\nREmL0oEMlrT9VbCPslXQBQAA8oABITFKBj3XymiZkby9qS0pDyeIfvFwLuAArD6S\nYefhPEhqH2LtjshiVN4BZpB956lHGV9wZ0BrhnhcKDC04hFqvGiFr43gkJrRnM9g\nROOPMaxSQG7iVvqebs9e7VJM+zN0u15TyrrhRIngrMYKVCyQewlrtK9xL86L43LE\nuDCoHGlGW7+lCXQ+uThUEdr+3vDZlKVMFp0CUxZce5aQPioyQJxmzWGfXD9iJhwa\nbl7HYETIfGdXBprS/HHn89hZJNPf7nFrOroWpPNJKW+YWC/mgGt06ogT6CnHOQrW\naVMG4jSnZD1Hk+fr95RoV5CHkhrbsoiLPf128YpN5cM8cTsRb+Gn\n-----END CERTIFICATE REQUEST-----\n"}
[00:00:08.925,537] <inf> rsa: Example finished successfully!
```
