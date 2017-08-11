#!/bin/bash

if [ $# != 2 ]
then
	echo "Required args: lab # and number of excersises."
	exit 1
fi
PWD=$(pwd)
LAB="cw"$1
EXC_COUNT=$2
cd GrocholaPawel/
mkdir $LAB
cd $LAB

for I in $(seq 1 $EXC_COUNT)
do 
	EXC="zad"$I
	CMAKE_PROJECT=$LAB$EXC
	mkdir $EXC
	cp "../template/CMakeLists_main.txt" "$EXC/CMakeLists.txt"

	PATTERN_PROJECT="s/__PROJECT__/${CMAKE_PROJECT}/g"
	PATTERN_TARGET="s/__TARGET__/${EXC}/g"
	sed -i -e "$PATTERN_PROJECT" "$EXC/CMakeLists.txt"
	sed -i -e "$PATTERN_TARGET" "$EXC/CMakeLists.txt"

	mkdir "$EXC/src/"
	cp "../template/CMakeLists_src.txt" "$EXC/src/CMakeLists.txt"
	cp "../template/main.c" "$EXC/src/main.c"
done

cd $PWD
