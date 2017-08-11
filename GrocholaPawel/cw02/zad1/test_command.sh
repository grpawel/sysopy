#!/bin/bash
#LIBRARY FUNCTIONS
command=$1
count=$2
size=$3
filename=$4
cp $filename test/file_copy1
cp $filename test/file_copy2
echo "Record size=$size, number=$count, sort lib"
time bin/zad1 $command lib test/file_copy1 $count $size
echo "Record size=$size, number=$count, sort sys"
time bin/zad1 $command sys test/file_copy2 $count $size