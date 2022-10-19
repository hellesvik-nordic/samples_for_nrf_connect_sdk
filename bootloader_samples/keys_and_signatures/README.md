# Keys and Signatures
If you build a sample for MCUboot, for example my [Simple SMP Sample](../smp/mcuboot_smp), you might have seen the following warning in the build log:
```
          ---------------------------------------------------------
          --- WARNING: Using default MCUBoot key, it should not ---
          --- be used for production.                           ---
          ---------------------------------------------------------
```

This folder has samples for fixing this, and for other key/signature related use-cases of bootloaders.

## Frequently Asked Questions about MCUboot signing and such

Q. Do I need to sign images manually using [imgtool.py](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/mcuboot/imgtool.html)?
A. Most likely no. Building files will automatically generate signed files for you in build/zephyr. See [Using MCUboot in nRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/mcuboot/readme-ncs.html).

# Theory
MCUboot has some detailed documentation on how its built, including security features at [MCUboot docs](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/mcuboot/wrapper.html).
The nRF Connect SDK also has some information about bootloaders and security at [Secure Boot](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/nrf/security_chapter.html#secure-boot).
Before reading further, I recommend having a look at the more basic theory under [Bootloader samples](../).

