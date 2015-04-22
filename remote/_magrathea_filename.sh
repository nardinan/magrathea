#!/bin/bash
# $1 > <op>_directory_destination
# $2 > <op>
# examples:
# source _magrathea_filename.sh $dld_data_folder "dld"
if [ "$1" ] && [ "$2" ]; then
	source _magrathea_run_number.sh
	source _magrathea_timer.sh
	destination_folder=$1"/run_"$run_filename
	echo "[*] destination directory selected and injected: $destination_folder"
	if [ ! -d $destination_folder ]; then
		mkdir $destination_folder
		final_filename=$destination_folder"/"$2$run_filename"_"$date_filename"_"$time_filename
		echo "[*] filename selected and injected: $final_filename"
	else
		echo -e "[!] \e[93mWarning!\e[0m directory '$destination_folder'already exists. Maybe here we have a  duplicated RUN ID?"
		exit 0
	fi
fi
