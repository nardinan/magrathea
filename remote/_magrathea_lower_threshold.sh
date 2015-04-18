#!/bin/bash
magrathea_lower_threshold_file="./lower_threshold_number.io"
lower_threshold_code=("03" "04" "05")
lower_threshold_human_readable=("3" "4" "5")
lower_threshold_number=0
lower_threshold_restart=0
if [ -f $magrathea_lower_threshold_file ]; then
	lower_threshold_number=`cat $magrathea_lower_threshold_file`
	lower_threshold_number=$(($lower_threshold_number+1))
fi
if [ "$lower_threshold_number" -gt $((${#lower_threshold_code[@]}-1)) ]; then
	lower_threshold_number=0
	lower_threshold_restart=1
fi
echo $lower_threshold_number > $magrathea_lower_threshold_file
echo "[*] lower threshold selected and injected: ${lower_threshold_human_readable[$lower_threshold_number]}"
$magrathea_command write -x a2f0${lower_threshold_code[$lower_threshold_number]}
$magrathea_command write -x a2f1${lower_threshold_code[$lower_threshold_number]}
$magrathea_command write -x a2f2${lower_threshold_code[$lower_threshold_number]}
$magrathea_command write -x a2f3${lower_threshold_code[$lower_threshold_number]}
$magrathea_command write -x a2f4${lower_threshold_code[$lower_threshold_number]}
$magrathea_command write -x a2f5${lower_threshold_code[$lower_threshold_number]}
echo "|| --- | $date_human_readable $time_human_readable | --- | ALL | operation | "\
	"LOWER THRESHOLD has been replaced with ${lower_threshold_human_readable[$lower_threshold_number]} |" >> $log_file
