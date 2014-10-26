#!/bin/bash
for parameter in $@;
do
	cd ~/Projects/magrathea/convert
	for trb in 1 2; # add here all TRBs that you need
	do
		./magrathea_convert.bin $parameter $trb &
	done
done
