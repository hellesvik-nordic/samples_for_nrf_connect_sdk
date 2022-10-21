# nRF5340 Theory
Yes, the firmware update of the nRF5340 is complicated enough that I have its own README for just it.

## Do I need to actually know the inner workings of the nRF5340 to do Firmware Update on it?
No, not really.  
But it will help if you need to start debugging.

## Why is DFU more complicated for the nRF5340?
Because it has two cores, and both cores should be upatable.

![nRF5340](../../.images/nrf5340_bootloaders.png)

As you can see from this imahe, be net core flash and the app core flash are separated.  
Therefore, you can not just swap an image from the Secondary Slot to the net core flash.  
As you might guess from the figure, the solution to this is to use the shared SRAM. I will explain how later on.

## Two bootloaders
The nRF5340 has two bootloaders.  
Usually MCUboot for for the app core.  
[b0n](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.1.0/nrf/samples/nrf5340/netboot/README.html) for the net core. 


