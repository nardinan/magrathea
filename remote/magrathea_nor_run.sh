#!/bin/bash
source _magrathea_environment.sh
source _magrathea_filename.sh $nor_data_folder "nor"
source _magrathea_trigger.sh
$magrathea_command stream -o $final_filename
$magrathea_command write -x 040500
$magrathea_command trigger $trigger_command
echo "[!] RUN"
# log
echo "|| $run_filename | $date_human_readable $time_human_readable | $trigger_log | ALL | NOR ||" >> $log_file
