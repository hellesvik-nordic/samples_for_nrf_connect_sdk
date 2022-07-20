from pynrfjprog import LowLevel
# FILL IN YOUR INFO
my_snr = 1050084572
target_cp = LowLevel.CoProcessor.CP_APPLICATION
print("Start")
with LowLevel.API("NRF53", log=True) as nrf:
    print("Connecting to debugger")
    nrf.connect_to_emu_with_snr(my_snr)
    # Connect debugger to nrf device and select target processor
    nrf.connect_to_device()
    nrf.enable_coprocessor(target_cp)
    nrf.select_coprocessor(target_cp)
    # Enable eraseprotect
    nrf.enable_eraseprotect()
    print("Done")
