# NSIB Simple sample
Simplest possible(I hope) sample for using NSIB.
Build and flash the program using west.

Then increment CONFIG_FW_INFO_FIRMWARE_VERSION.
Build the program again using west. Do not flash this time.

Program the new image to the s0 slot by using:
```
nrfjprog --reset --sectorerase --program build/zephyr/signed_by_b0_s0_image.hex
```
