#!/bin/bash
for parameter in $@;
do
	cd ~/Projects/magrathea/viewer
	for trb in 0 1 2 3 4 5 6 7; # add here all TRBs that you need
	do
		binary_name=$parameter"$trb.bin"
		./magrathea_view.bin $binary_name $trb 0 512 -x -k -r
	done
	# this piece of code has been used during EQM tests in Shanghai (9-18 Sept 2014)
	# cd ~/Projects/magrathea/compare
	#./magrathea_compare.bin ~/Desktop/calibrations_TRB1 "$parameter"_TRB01cals 2,3,6,7,10,11,14,15,18,19,22,23 # 21 has been removed because it is bad
	#./magrathea_compare.bin ~/Desktop/calibrations_TRB2 "$parameter"_TRB02cals 0,1,4,5,8,9,12,13,16,17,20,21,22
	# cd ~/Projects/magrathea/
done
