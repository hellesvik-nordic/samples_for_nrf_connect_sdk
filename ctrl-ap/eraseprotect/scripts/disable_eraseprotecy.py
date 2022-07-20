from pynrfjprog import LowLevel
# FILL IN YOUR INFO
my_snr = 1050084572
eraseprotect_key = 0x12345678
target_cp = LowLevel.CoProcessor.CP_APPLICATION
# FROM DATASHEET
# CTRL-AP ID according to datasheet debug and trace (2 = app, 3 = net)
ap_id = 2 if target_cp == LowLevel.CoProcessor.CP_APPLICATION else 3
# CTRL-AP addr offsets.
eraseprotect_disable_addr = 0x01C
print("Start")
with LowLevel.API("NRF53", log=True) as nrf:
    print("Connecting to debugger")
    # Connect to debugger/emulator
    nrf.connect_to_emu_with_snr(my_snr)
    # Write values
    print(f"{eraseprotect_key:#010x} --> {eraseprotect_disable_addr:#05x}")
    nrf.write_access_port_register(ap_id, eraseprotect_disable_addr, eraseprotect_key)
    print("Done")
