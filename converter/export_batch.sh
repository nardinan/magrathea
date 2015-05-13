#!/bin/bash
script_path=`echo "$(readlink -f "$0")"|sed -e "s/\/[^\/]*$//"`
cd $script_path
for parameter in $@;
do
	for trb in 0 1 2 3 4 5 6 7; # add here all TRBs that you need
	do
		./magrathea_convert.bin $parameter $trb -r &
	done
done
