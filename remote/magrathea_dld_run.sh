#!/bin/bash
source _magrathea_environment.sh
source _magrathea_run_number.sh
source _magrathea_timer.sh
trigger_log="I:100Hz"
trigger_human_readable="100 Hz, internal trigger"
trigger_command="-s 100"
final_filename=$dld_data_folder"/dld"$postfix_filename
echo "[*] trigger selected and injected: $trigger_human_readable"
echo "[*] filename selected and injected: $final_filename"
$magrathea_command stream -o $final_filename
$magrathea_command write -x 040b00
$magrathea_command trigger $trigger_command
echo "[!] RUN" 
# log
echo "|| $run_filename | $date_human_readable $time_human_readable | $trigger_log | ALL | PED&DLD ||" >> $log_file