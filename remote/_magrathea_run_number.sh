#!/bin/bash
run_number=0
if [ -f $magrathea_run_number_file ]; then
	run_number=`cat $magrathea_run_number_file`
	run_number=$(($run_number+1))
fi
echo $run_number > $magrathea_run_number_file
run_filename=`printf %04d $run_number`
