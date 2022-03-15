# MCUBoot Simple Sample
Simplest possible(I hope) sample for using NSIB.
Build and flash the program using west.

Build the program again using west. Do not flash this time.

Program the new image using:
```
nrfjprog --reset --sectorerase --program build/zephyr/app_signed.hex
```

See [Using MCUboot in NRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.9.1/mcuboot/readme-ncs.html) for more informationon.
