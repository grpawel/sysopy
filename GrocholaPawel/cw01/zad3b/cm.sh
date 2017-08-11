#!/bin/bash
values=( O0 O1 O2 O3 Os )
file="../zad3b/results.txt"
for i in "${values[@]}"
do
	echo "" >> $file
	echo "Optimalization level "$i >> $file
	cmake -DCMAKE_BUILD_TYPE=$i
	make
	make test >> $file
done