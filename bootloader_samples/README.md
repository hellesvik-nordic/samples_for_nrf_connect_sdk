# Bootloader Samples
These are my bootloader samples.

The official Bootloader sample from the nRF Connect SDK is the [SMP Server Sample](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/zephyr/samples/subsys/mgmt/mcumgr/smp_svr/README.html). That one is properly tested.

For some proper theory on Bootloaders and Device Firmware Update (DFU), see the nRF Connect SDK on [Security](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/nrf/security_chapter.html).

## Quick Start
Are you here to just get the simplest possible bootloader sample work with a nRF chip?

Start with the [Simple MCUboot SMP sample](smp/mcuboot_smp/)

## NSIB vs MCUboot
In general, the nRF Connect SDK uses MCUboot for its bootloader.

If you need an Upgradable Bootloader, the Nordic Secure Immutable Bootloader (NSIB) is generally used in addition to MCUboot.

For more information on this, see [Bootloaders and Device Firmware Updates](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/nrf/app_bootloaders.html#app-bootloaders).

## SMP Server and Serial Recovery
There are two versions of updating a device using MCUboot: SMP Server and Serial Recovery.  
Samples for these can be found in [Samples for SMP Server](smp/) and [Samples for Serial Recovery](serial_recovert/), respectivley.
