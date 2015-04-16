#!/bin/bash
folder_name="" # here your folder name
if [ -n "$folder_name"]; then
	echo "[error] you forgot the folder name"
	exit
fi
magrathea_run="echo ./magrathea_remote.bin 127.0.0.1 8090"
remote_log="operations.log"
# 1 - configure HD and status
# 2 - RAW mode [ 12 mins @ int 100 Hz ]
# 3 - PED mode [  2 mins @ int 100 Hz ]
# 4 - NOR mode [ 60 mins @ ext        ]
hold_delay=("82" "8C" "8C" "96" "96" "78" "78" "A0" "A0" "6E" "AA" "64")
hold_delay_human_readable=("6.5 uS" "7 uS" "7 uS" "7.5 uS" "7.5 uS" "6 uS" "6 uS" "8 uS" "8 uS" "5.5 uS" "8.5 uS" "5 uS")
current_hold_delay=0
current_status=0
current_loop=0
current_run=34
$magrathea_run trigger -off
$magrathea_run mask -a
while [ 1 ]; do
	if [ "$current_status" -gt 3 ]; then
		echo "end of the loop $current_loop"
		current_loop=$((current_loop+1))
		current_status=1;
	else
		current_status=$((current_status+1))
	fi
	date_human_readable=`date +"%y/%m/%d"`
	time_human_readable=`date +"%H:%M"`
	date_name=`date +"%y%m%d"`
	time_name=`date +"%H%M"`
	this_name=`printf %04d $current_run`
	case "$current_status" in
		1)
			echo "| --- | $date_human_readable $time_human_readable | set hold delay to ${hold_delay_human_readable[$current_hold_delay]} |||||" \
				>> $remote_log
			$magrathea_run stream -o /dev/null
			$magrathea_run write -x 08${hold_delay[$current_hold_delay]}00
			current_hold_delay=$((current_hold_delay+1))
			if [ "$current_hold_delay" -gt 11 ]; then
				current_hold_delay=0
			fi
			$magrathea_run status -d 0
			$magrathea_run status -d 1
			$magrathea_run status -d 2
			$magrathea_run status -d 3
			$magrathea_run status -d 4
			$magrathea_run status -d 5
			$magrathea_run status -d 6
			$magrathea_run status -d 7
			;;
		2) 
			echo "| $this_name | $date_human_readable $time_human_readable | I: 100Hz || ALL | RAW ||" >> $remote_log;  
			$magrathea_run stream -o raw"$this_name"_"$date_name"_"$time_name"
			$magrathea_run write -x 040700
			$magrathea_run trigger -s 100
			sleep 720 # 12 minutes
			$magrathea_run trigger -off
			$magrathea_run stream -o /dev/null
			;;
		3)
			echo "| $this_name | $date_human_readable $time_human_readable | --- || ALL | PED&DLD ||" >> $remote_log;
			$magrathea_run stream -o dld"$this_name"_"$date_name"_"$time_name"
			$magrathea_run write -x 040b00
			$magrathea_run trigger -s 100
			sleep 120 # 2 minutes
			$magrathea_run trigger -off
			$magrathea_run stream -o /dev/null
			;;
		4)	
			echo "| $this_name | $date_human_readable $time_human_readable | Ext cosmics || ALL | NOR ||" >> $remote_log;
			$magrathea_run stream -o nrm"$this_name"_"$date_name"_"$time_name"
			$magrathea_run write -x 040500
			$magrathea_run trigger -s 100
			sleep 3600 # 60 minutes
			$magrathea_run trigger -off
			$magrathea_run stream -o /dev/null
			;;
	esac
	current_run=$((current_run+1));
done
