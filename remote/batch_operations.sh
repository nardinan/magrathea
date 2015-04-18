#!/bin/bash
current_status=0
current_loop=0
source _magrathea_environment.sh
source _magrathea_timer.sh
source _magrathea_common_noise.sh
source _magrathea_higher_threshold.sh
while [ 1 ]; do
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
				source _magrathea_common_noise.sh
				if [ "$common_noise_restart" -gt "0" ]; then
					source _magrathea_higher_threshold.sh
				fi
			fi
			# custom operations
			;;
		2)
			trigger_log="I:100Hz"
			trigger_human_readable="100 Hz, internal trigger"
			trigger_command="-s 100"
			source magrathea_raw_run.sh
			sleep 240 # 4 minutes
			;;
		3)	
			source magrathea_dld_run.sh
			sleep 120 # 2 minutes
			;;
		4)
			trigger_log="Ext"
			trigger_human_readable="external trigger"
			trigger_command="-ext"
			source magrathea_nor_run.sh
			sleep 900 # 15 minutes
			;;
	esac
done
