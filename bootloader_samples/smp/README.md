# SMP Samples
The [Simple Management Protocol (SMP)](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/zephyr/services/device_mgmt/smp_protocol.html) is an application layer protocol provided by [mcumgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/zephyr/services/device_mgmt/mcumgr.html).  
SMP is transport layer agnostic, so you can run it on top of any transport protocol. But in the nRF Connect SDK, UART and Bluetooth Low Energy are mostly used for this.

For an official sample, see the [Zephyr SMP Server sample](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/zephyr/samples/subsys/mgmt/mcumgr/smp_svr/README.html). 
I created a minimal version of a SMP Server, with only serial communication and Image Management for DFU at [MCUboot Simple sample](./mcuboot_smp)


# Theory
There are some official documentation on DFU in the nRF Connect SDK under [Device Firmware Upgrade](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/nrf/security_chapter.html#device-firmware-upgrade-dfu)

## What is DFU and why do we need it?
A Device Firmware Upgrade (DFU) is when new firmware is sent to a device, updating it.  
When developing, you can program a chip using a Debugger. A Debugger can write to more or less any register in the chip.  
DFU on the other hand can only send an image to the application running on a device. Then the application can write that image to flash.  
But the application can not write to the slot itself runs in, as that would break the application. 
Therefore, we need a **second slot** to write the new image to, as such:  
![Sent To Secondary Slot](../../.images/new_firmware.png)

## DFU is not the same as a bootloader
DFU is to send new firmware to a slot in flash.
This is often handeled by the application.  
The bootloader do not need to be involved in the DFU.  
The bootloader will read what is in its slots on boot, but does not care about how the slots were populated.  
It is possible to have a chip with a bootloader and not DFU functionality. (For example [NSIB Simple sample](../updatable_bootloader/nsib_simple))

However, It is possible for a bootloader to have functionality for DFU. See [Serial Recovery](../serial_recovery) for more information on this.

## mcumgr and SMP
To transfer new firmware to the application, the nRF Connect SDK supports the [Simple Management Protocol (SMP)](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/zephyr/services/device_mgmt/smp_protocol.html).  
A SMP Server will run on the chip to be upgraded, and a SMP Client will run on the "upgrader", typically a PC:  
![SMP Client and Server](../../.images/smp_client_server.png)

SMP is transport layer agnostic, so you can run it on top of any transport protocol. But in the nRF Connect SDK, UART and Bluetooth Low Energy are mostly used for this.

The Zephyr Project supplies [mcumgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/zephyr/services/device_mgmt/mcumgr.html) as a SMP Client.  
mcumgr supports transport over Bluetooth Low Energy, UART and UDP over IP.

Nordic Semiconductor supplies the [nRF Connect for Mobile app](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile) and the [nRF Device Firmware Update app](https://play.google.com/store/apps/details?id=no.nordicsemi.android.dfu&gl=US) for mobile phones as SMP Clients.  
We can use either of these applications to perform DFU over Bluetooth Low Energy.

SMP functionality for DFU is called Image Management. 
SMP has other functionality as well, such as file transfer, but I will not cover these.

It is possible to use another nRF chip as a SMP Client. My colleague has a sample for this in his [Bluetooth: Central SMP Client DFU sample](https://github.com/simon-iversen/ncs_samples/tree/master/central_smp_client_dfu).

