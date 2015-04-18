#!/bin/bash
magrathea_common_noise_file="./common_noise_number.io"
common_noise_code=("14", "28", "32", "1e")
common_noise_human_readable=("20", "40", "50", "30")
common_noise_number=0
common_noise_restart=0
if [ -f $magrathea_common_noise_file ]; then
	common_noise_number=`cat $magrathea_common_noise_file`
	common_noise_number=$(($common_noise_number+1))
fi
if [ "$common_noise_number" -gt $((${#common_noise_code[@]}-1)) ]; then
	common_noise_number=0
	common_noise_restart=1
fi
echo $common_noise_number > $magrathea_common_noise_file
echo "[*] common noise selected and injected: ${common_noise_human_readable[$common_noise_number]}"
$magrathea_command write -x a0${common_noise_code[$common_noise_number]}00
echo "|| --- | $date_human_readable $time_human_readable | --- | ALL | operation | "\
	"COMMON NOISE CUT has been replaced with ${common_noise_human_readable[$common_noise_number]} |" >> $log_file
