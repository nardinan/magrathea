#!/bin/bash
for parameter in $@;
do
	./magrathea_view.bin $parameter 1 0 512 -x -D
	./magrathea_view.bin $parameter 2 0 512 -x -D
done
