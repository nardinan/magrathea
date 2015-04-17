#!/bin/bash
magrathea_hold_delay_file="./hold_delay_number.io"
hold_delay_code=("82" "8C" "8C" "96" "96" "78" "78" "A0" "A0" "6E" "AA" "64")
hold_delay_human_readable=("6.5 uS" "7 uS" "7 uS" "7.5 uS" "7.5 uS" "6 uS" "6 uS" "8 uS" "8 uS" "5.5 uS" "8.5 uS" "5 uS")
hold_delay_number=0
if [ -f $magrathea_hold_delay_file ]; then
	hold_delay_number=`cat $magrathea_hold_delay_file`
	hold_delay_number=$(($hold_delay_number+1))
fi
if [ "$hold_delay_number" -gt $((${#hold_delay_code[@]}-1)) ]; then
	hold_delay_number=0
fi
echo $hold_delay_number > $magrathea_hold_delay_file
echo "[hold delay selected and injected: ${hold_delay_human_readable[$hold_delay_number]} ]"
$magrathea_command write -x 08${hold_delay_code[$hold_delay_number]}
echo "|| --- | $date_human_readable $time_human_readable | --- | ALL | operation | "\
	"HD has been replaced with ${hold_delay_human_readable[$hold_delay_number]} |" >> $log_file
