#!/bin/bash
current_status=0
current_loop=0
while [ 1 ]; do
	source _magrathea_header.sh
	source _magrathea_timer.sh
	if [ "$current_status" -gt 3 ]; then
		echo "[---] end of the loop $current_loop [---]"
		current_loop=$((current_loop+1))
		current_status=1;
	else
		current_status=$((current_status+1))
	fi
	case "$current_status" in
		1)
			# custom operations
			;;
		2)
			trigger_log="I:100Hz"
			trigger_human_readable="100 Hz, internal trigger"
			trigger_command="-s 100"
			source magrathea_raw_run.sh
			sleep 2 #720 # 12 minutes
			;;
		3)	
			source magrathea_dld_run.sh
			sleep 2 #120 # 2 minutes
			;;
		4)
			trigger_log="Ext"
			trigger_human_readable="external trigger"
			trigger_command="-ext"
			source magrathea_nor_run.sh
			sleep 2 #3600 # 60 minutes
			;;
	esac
done
