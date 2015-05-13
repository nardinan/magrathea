#!/bin/bash
script_path=`echo "$(readlink -f "$0")"|sed -e "s/\/[^\/]*$//"`
cd $script_path
trb_id=0
for parameter in $@;
do
	for ladder in $(seq 0 23); # add here all TRBs that you need
	do
		./firefly_exporter.bin $parameter $trb_id $ladder &
	done
done
