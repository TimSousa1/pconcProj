#!/bin/bash

if [ $# -ne 2 ]; then
    echo "usage: ./run.sh <datasets_folder> <max_threads>"
    exit 1
fi

for f in $1/*.csv; do mv "$f" "$(echo "$f" | sed s/csv/csv.bak/)"; done

for folder in $1/*
do
    echo "rm $folder/*.csv"
    rm $folder/*.csv

    for ((i=1; i<=$2; i*=2))
    do
        echo "rm -r $folder/old_photo_PAR_B"
        rm -r $folder/old_photo_PAR_B

        echo "./old-photo-paralelo-B $folder $i"
        ./old-photo-paralelo-B $folder $i
    done

    echo "awk '(NR == 1) || (FNR > 1)' $folder/*.csv > $folder.csv"
    awk '(NR == 1) || (FNR > 1)' $folder/*.csv > $folder.csv

done
