#!/bin/bash
source _magrathea_environment.sh
source _magrathea_run_number.sh
source _magrathea_timer.sh
source _magrathea_trigger.sh
final_filename=$nor_data_folder"/nor"$postfix_filename
echo "[*] filename selected and injected: $final_filename"
$magrathea_command stream -o $final_filename
$magrathea_command write -x 040500
$magrathea_command trigger $trigger_command
echo "[!] RUN"
# log
echo "|| $run_filename | $date_human_readable $time_human_readable | $trigger_log | ALL | NOR ||" >> $log_file
