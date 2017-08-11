#!/bin/bash
CW=$1
EXCLUDES=
for d in GrocholaPawel/*
do
	if [ "$d" != GrocholaPawel/"$CW" ]; then
		EXCLUDES="$EXCLUDES""--exclude ""$d "
	fi
done
tar -pczf GrocholaPawel-"$CW".tar.gz GrocholaPawel $EXCLUDES