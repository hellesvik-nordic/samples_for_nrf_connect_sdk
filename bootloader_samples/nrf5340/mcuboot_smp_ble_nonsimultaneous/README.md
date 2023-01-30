# Non-Simultaneous update of both the Application-core and the Network-core

## nRF5340DK Specific
The nRF5340DK have two COM-ports. 
Make sure to specify the COM-port for the Application core for updates.

## DFU over UART
See [The Simple SMP sample](../../smp/mcuboot_smp_uart). 
This sample can be used to update both cores in order. 
To change the upload for the network core, use 
```
mcumgr -c acm1 image upload build/zephyr/net_core_app_update.bin
```

## DFU over Bluetooth Low Energy
See [MCUboot SMP Sample feat Bluetooth Low Energy sample](../../smp(mcuboot_smp_ble). 
As above, use app\_update.bin for the application core, and net\_core\_app\_update.bin for the network core.

