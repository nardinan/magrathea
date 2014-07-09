#!/bin/bash
echo "[*] checking RS422 device ..."
cd $RS422_DRIVER_DIR/drivers
sudo ./3544_inst.pl -cards 2 &>/dev/null
echo "[*] checking LVDS & Trigger devices ..."
cd $PLX_SDK_DIR/Bin
sudo -E bash -c './Plx_load 9656 &>/dev/null'
sudo -E bash -c './Plx_load 9054 &>/dev/null'
echo "[*] done"

