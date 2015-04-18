#!/bin/bash
source _magrathea_header.sh
source _magrathea_timer.sh
source _magrathea_trigger.sh
final_filename=$nor_data_folder"/nor"$postfix_filename
echo "[magrathea filename: $final_filename]"
$magrathea_command stream -o $final_filename
$magrathea_command write -x 040500
echo "[configuring $trigger_human_readable]"
$magrathea_command trigger $trigger_command
echo "[running ...]"
# log
echo "|| $run_filename | $date_human_readable $time_human_readable | $trigger_log | ALL | NOR ||" >> $log_file
