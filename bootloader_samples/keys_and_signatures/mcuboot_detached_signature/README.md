#MCUboot detached signature DFU

See the scripts/ folder for how to sign with detached signatures. This script should be quite self-explanetory. Let me know if it is not.

This sample uses UART for transport, so see [Simple SMP Sample over UART](../../smp/mcuboot_smp_uart) for how to do DFU with it.


## Explanation
This sample has `scripts/detached_signature.sh` that shows what you can do on your PC, but then it also has `scripts/black_box` that uses openssl with generic cryptographic operations, showing how a black box can be used alongside imgtool to sign your MCUboot updates without having direct access to your private key.
