#!/bin/bash
magrathea_command="echo ./magrathea_remote.bin 127.0.0.1"
magrathea_run_number_file="./run_number.io"
log_file="./operations.log"
raw_data_folder="" # RAW datafile destination
nor_data_folder="" # NOR datafile destination
dld_data_folder="" # DLD datafile destination
run_number=0
if [ -f $magrathea_run_number_file ]; then
	run_number=`cat $magrathea_run_number_file`
	run_number=$(($run_number+1))
fi
echo $run_number > $magrathea_run_number_file
run_filename=`printf %04d $run_number`
if [ ! "$raw_data_folder" ]; then
	echo "NO RAW data folder";
	exit 0;
fi
if [ ! "$nor_data_folder" ]; then
	echo "NO NOR data folder";
	exit 0;
fi
if [ ! "$dld_data_folder" ]; then
	echo "NO DLD data folder";
	exit 0;
fi
echo "[run number: $postfix_filename]"
echo "[magrathea is configuring his environment ...]"
$magrathea_command trigger -off
$magrathea_command mask -a
$magrathea_command stream -o /dev/null
for trb_id in 0 1 2 3 4 5 6 7; do
	$magrathea_command status -d $trb_id
done
