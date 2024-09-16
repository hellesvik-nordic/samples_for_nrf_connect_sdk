# Netcore Serial Recovery with Sysbuild

Tested with the nRF5340DK and nRF Conenct SDK v2.7.0.  
To save space, we overlap mcuboot\_secondary slots, as seen in pm\_static.yml.  
The nRF Connect SDK does not support updating the network core without a secondary slot, so we need at least one secondary slot.

## Testing procedure
Add a printk statement to the main.c of the [HCI IPC sample](https://github.com/nrfconnect/sdk-zephyr/tree/main/samples/bluetooth/hci_ipc). Then build and flash this serial recovery sample to your nRF5340DK with Sysbuild. Either use VS Code, or the CLI as such:
```
west build -p -b nrf5340dk/nrf5340/cpuapp
west flash --erase --recover
```
Connect to serial terminal and verify the prints are as expected. Then disconnect the serial terminal.

Next up, we will DFU the device. Reset the DK while holding Button 1.  
This sample will use [mcumgr-client](https://github.com/vouch-opensource/mcumgr-client/) instead of the mcumgr cli used in other samples in here.
Then check that you can list images. ACM port may change, and the port will likely be COM on Windows.
See [Sysbuild: Filename Changes](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/releases_and_maturity/migration/migration_sysbuild.html#filename_changes) for other filenames used in this sample.
```
mcumgr-client -d /dev/ttyACM1 list
```
Before doing the DFU, change prints in both app and hci\_ipc. If not, the DFU will detect that no changes has been made and likely be ignored. Or at the least, we want to be able to observe that it actually wored.  
First upload netcore image. See -s 3 to select the secondary slot.
```
mcumgr-client -s 3 -d /dev/ttyACM1 upload build/signed_by_mcuboot_and_b0_hci_ipc.bin
```
Next the app image, straight into the primary slot:
```
mcumgr-client -s 1 -d /dev/ttyACM1 upload build/mcuboot_netcore_serial_recovery/zephyr/zephyr.signed.bin 
```

Next reset the device and verify from the logs that this worked.
