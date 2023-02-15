# WORK IN PROGRESS
THE FOLLOWING PROJECT IS NOT FINISHED AND WILL LIKELY NOT WORK

# SMP Client over UART
This sample is mostly a changed version of [SMP Client over BLE](../smp_client_ble).

## Testing
This sample is tested with two nRF52840DK's and NCS v2.2.0
For testing, we need a SMP Server over UART. Copy app.overlay to  [SMP Server over UART](../../smp/mcuboot_smp_uart) and run it on a secondary nRF52840DK.  
Then connect the SMP Client and SMP Server together with P0.00 and P0.01. Remember to connect TX-RX.

