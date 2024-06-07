from pynrfjprog import LowLevel
# FILL IN YOUR INFO
my_snr = 1050084572
target_cp = LowLevel.CoProcessor.CP_APPLICATION
# FROM DATASHEET
# CTRL-AP ID according to datasheet debug and trace (2 = app, 3 = net)
ap_id = 2 if target_cp == LowLevel.CoProcessor.CP_APPLICATION else 3
# CTRL-AP addr offsets.
eraseprotect_status_addr = 0x018

print("Start")
with LowLevel.API("NRF53", log=True) as nrf:
    print("Connecting to debugger")
    nrf.connect_to_emu_with_snr(my_snr)
    # Write values
    status = nrf.read_access_port_register(ap_id, eraseprotect_status_addr)
    print("ERASEPROTECT.STATUS: ", status)
