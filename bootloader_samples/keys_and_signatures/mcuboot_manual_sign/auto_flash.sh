#!/bin/bash
west flash --erase
nrfjprog --sectorerase --reset --program app_manually_signed.hex
