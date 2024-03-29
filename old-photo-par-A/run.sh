#!/bin/bash

if [ $# -ne 2 ]; then
    echo "usage: ./run.sh <datasets_folder> <max_threads>"
    exit 1
fi

for folder in $1/*
do
    echo "rm $folder/*.csv"
    rm $folder/*.csv

    for ((i=1; i<=$2; i*=2))
    do
        echo "rm -r $folder/old_photo_PAR_A"
        rm -r $folder/old_photo_PAR_A

        echo "./old-photo-paralelo-A $folder $i"
        ./old-photo-paralelo-A $folder $i
    done

    echo "awk '(NR == 1) || (FNR > 1)' $folder/*.csv > $folder.csv"
    awk '(NR == 1) || (FNR > 1)' $folder/*.csv > $folder.csv

done
