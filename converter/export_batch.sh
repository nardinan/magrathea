#!/bin/bash
for parameter in $@;
do
	cd ~/Projects/magrathea/convert
	for trb in 0 1 2 3 4 5 6 7; # add here all TRBs that you need
	do
		./magrathea_convert.bin $parameter $trb &
	done
done
