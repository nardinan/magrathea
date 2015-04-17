#!/bin/bash
echo "[trigger selection]"
if [ ! "$trigger_command" ]; then
	kind=""
	while [ "$kind" != "i" ] && [ "$kind" != "e" ]; do
		read -r -s -p "[uhm ... what do you want? Internal or External trigger? (i/e)]" -n 1 kind
		echo " $kind"
	done
	if [ "$kind" == "i" ]; then
		speed=""
		while [ "$speed" != "1" ] && [ "$speed" != "2" ] && [ "$speed" != "3" ] && [ "$speed" != "4" ]; do
			read -r -s -p "[ok, which speed? (1 - 50Hz | 2 - 100Hz | 3 - 200Hz | 4 - 300Hz)]" -n 1 speed
			echo " $speed"
		done
		case "$speed" in
			1)
				trigger_log="I:50Hz"
				trigger_human_readable="50 Hz, internal trigger"
				trigger_command="-s 50"
				;;
			2)
				trigger_log="I:100Hz"
				trigger_human_readable="100 Hz, internal trigger"
				trigger_command="-s 100"
				;;
			3)
				trigger_log="I:200Hz"
				trigger_human_readable="200 Hz, internal trigger"
				trigger_command="-s 200"
				;;
			4)
				trigger_log="I:300Hz"
				trigger_human_readable="300 Hz, internal trigger"
				trigger_command="-s 300"
				;;
		esac
	else
		trigger_log="Ext"
		trigger_human_readable="external trigger"
		trigger_command="-ext"
	fi
fi
