#!/bin/bash
magrathea_higher_threshold_file="./higher_threshold_number.io"
higher_threshold_directory="" # configure this directory
if [ ! "$higher_threshold_directory" ]; then
	echo "NO HIGHER THRESHOLD data folder"
	exit 0
fi
higher_threshold_code=("inj0070-00_25" "inj0070-00_35" "inj0070-00_50")
higher_threshold_human_readable=("2.5 sigma (run 0070; no bad channels)" "3.5 sigma (run 0070; no bad channels)" "5 sigma (run 0070; no bad channels)")
higher_threshold_number=0
higher_threshold_restart=0
if [ -f $magrathea_higher_threshold_file ]; then
	higher_threshold_number=`cat $magrathea_higher_threshold_file`
	higher_threshold_number=$(($higher_threshold_number+1))
fi
if [ "$higher_threshold_number" -gt $((${#higher_threshold_code[@]}-1)) ]; then
	higher_threshold_number=0
	higher_threshold_restart=1
fi
echo $higher_threshold_number > $magrathea_higher_threshold_file
echo "[*] higher treshold selected and injected: ${higher_threshold_human_readable[$higher_threshold_number]}"
# this command will be very slow! We need about 10 minutes to be sure that everything works
echo "[*] sleeping for 6 minutes (just to give to the 'inject' command its time)"
$magrathea_command inject -f $higher_threshold_directory"/"${higher_threshold_code[$higher_threshold_number]}
sleep 360 # wait 6 minutes
echo "|| --- | $date_human_readable $time_human_readable | --- | ALL | operation | "\
	"HIGHER THRESHOLD has been replaced with ${higher_threshold_human_readable[$higher_threshold_number]} |" >> $log_file
