#!/bin/bash
current_status=0
current_loop=0
maximum_loop=-1
execute_higher_threshold=1
execute_raw_mode=1
execute_dld_mode=1
execute_nor_mode=1
# format is: batch_operatiions.sh <use raw><use dld><use nor> <update higher threshold> <loop has to runs for #>
if [ "$1" ] && [ "${#1}" -eq "4" ]; then
	if [ "$2" ]; then
		maximum_loop=$2;
		echo "[!] number of loops: $maximum_loop"
	fi
	execute_raw_mode=${1:0:1}
	execute_dld_mode=${1:1:1}
	execute_nor_mode=${1:2:1}
	execute_higher_threshold=${1:3:1}
	echo "[!] RAW MODE: $execute_raw_mode"
	echo "[!] DLD MODE: $execute_dld_mode"
	echo "[!] NOR MODE: $execute_nor_mode"
	echo "[!] HIGHER THRESHOLD: $execute_higher_threshold"
else
	echo "[!] usage: $0 <1/0 - run RAW><1/0 - run DLD><1/0 - run NOR><1/0 - update higher threshold> {number of loops}"
	exit 0;
fi
source _magrathea_environment.sh
source _magrathea_timer.sh
source _magrathea_hold_delay.sh
source _magrathea_common_noise.sh
if [ "$execute_higher_threshold" -eq "1" ]; then
	source _magrathea_higher_threshold.sh
fi
while [ "$maximum_loop" -eq "-1" ] || [ "$current_loop" -lt "$maximum_loop" ]; do
	if [ "$current_status" -gt 3 ]; then
		echo "[&] end of the loop number: $(($current_loop+1))"
		current_loop=$((current_loop+1))
		current_status=1;
	else
		current_status=$((current_status+1))
	fi
	case "$current_status" in
		1)
			source _magrathea_timer.sh
			source _magrathea_lower_threshold.sh
			if [ "$lower_threshold_restart" -gt "0" ]; then 
				source _magrathea_hold_delay.sh
				if [ "$hold_delay_restart" -gt "0" ]; then
					source _magrathea_common_noise.sh
					if [ "$execute_higher_threshold" -eq "1" ] && [ "$common_noise_restart" -gt "0" ]; then
						source _magrathea_higher_threshold.sh
					fi
				fi
			fi
			# custom operations
			;;
		2)
			if [ "$execute_raw_mode" -eq "1" ]; then
				trigger_log="I:100Hz"
				trigger_human_readable="100 Hz, internal trigger"
				trigger_command="-s 100"
				source magrathea_raw_run.sh
				sleep 240 # 4 minutes
				cp $magrathea_log_file $final_filename".log"
				cp $magrathea_raw_log_file $final_filename".raw.log"
				echo "" > $magrathea_log_file
				echo "" > $magrathea_raw_log_file
			fi
			;;
		3)	
			if [ "$execute_dld_mode" -eq "1" ]; then
				source magrathea_dld_run.sh
				sleep 120 # 2 minutes
				cp $magrathea_log_file $final_filename".log"
				cp $magrathea_raw_log_file $final_filename".raw.log"
				echo "" > $magrathea_log_file
				echo "" > $magrathea_raw_log_file
			fi
			;;
		4)
			if [ "$execute_nor_mode" -eq "1" ]; then
				trigger_log="Ext"
				trigger_human_readable="external trigger"
				trigger_command="-ext"
				source magrathea_nor_run.sh
				sleep 900 # 15 minutes
				cp $magrathea_log_file $final_filename".log"
				cp $magrathea_raw_log_file $final_filename".raw.log"
				echo "" > $magrathea_log_file
				echo "" > $magrathea_raw_log_file
			fi
			;;
	esac
done
