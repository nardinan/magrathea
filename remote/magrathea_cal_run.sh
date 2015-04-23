#!/bin/bash
source _magrathea_environment.sh
source _magrathea_filename.sh $cal_data_folder "cal"
source _magrathea_trigger.sh
calibration_code_H=("00" "01" "02" "04" "08" "0f")
calibration_code_L=("80" "00" "00" "00" "00" "ff")
calibration_human_readable=("128" "256" "512" "1024" "2048" "4095")
calibration_number=0
calibration_sleep_each_point=60 # 1 minute
while [ "$calibration_number" -lt ${#calibration_code_H[@]} ]; do
	current_final_filename=$final_filename"_DAC_"${calibration_human_readable[$calibration_number]}
	$magrathea_command stream -o $current_final_filename
	$magrathea_command write -x 0a${calibration_code_H[$calibration_number]}${calibration_code_L[$calibration_number]}
	$magrathea_command write -x 040900
	$magrathea_command trigger $trigger_command
	echo "[!] RUN (DAC ${calibration_human_readable[$calibration_number]})"
	sleep $calibration_sleep_each_point
	$magrathea_command trigger -off
	$magrathea_command stream -o /dev/null
	for trb_id in 0 1 2 3 4 5 6 7; do
		current_merged_filename=$final_filename"_TRB"$trb_id".bin"
		current_partial_filename=$final_filename"_DAC_"${calibration_human_readable[$calibration_number]}"_TRB"$trb_id".bin"
		if [ -f $current_partial_filename ]; then
			cat $current_partial_filename >> $current_merged_filename
		fi
	done
	# log
	echo "|| $run_filename | $date_human_readable $time_human_readable | $trigger_log | ALL | CAL "\
		"(dac ${calibration_human_readable[$calibration_number]}) ||" >> $log_file
	calibration_number=$(($calibration_number+1))
done
