#!/bin/bash
magrathea_command="echo ./magrathea_remote.bin 127.0.0.1 8090"
magrathea_run_number_file="./run_number.io"
magrathea_log_file="../App/magrathea.log"
magrathea_raw_log_file="../App/magrathea.raw.log"
log_file="./operations.log"
raw_data_folder="" # RAW datafile destination
nor_data_folder="" # NOR datafile destination
dld_data_folder="" # DLD datafile destination
cal_data_folder="" # CAL datafile destination
if [ ! "$raw_data_folder" ]; then
	echo "NO RAW data folder"
	exit 0
fi
if [ ! "$nor_data_folder" ]; then
	echo "NO NOR data folder"
	exit 0
fi
if [ ! "$dld_data_folder" ]; then
	echo "NO DLD data folder"
	exit 0
fi
if [ ! "$cal_data_folder" ]; then
	echo "NO CAL data folder"
	exit 0
fi
echo "[*] cleaning the environment ..."
$magrathea_command trigger -off
$magrathea_command mask -a
$magrathea_command stream -o /dev/null
for trb_id in 0 1 2 3 4 5 6 7; do
	$magrathea_command status -d $trb_id
done

