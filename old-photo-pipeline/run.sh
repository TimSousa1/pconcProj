#!/bin/bash

if [ $# -ne 1 ]; then
    echo "usage: ./run.sh <datasets_folder>"
    exit 1
fi

for folder in $1/*
do
    echo "rm $folder/*.csv"
    rm $folder/*.csv

    echo "rm -r $folder/old_photo_PIPELINE"
    rm -r $folder/old_photo_PIPELINE

    echo "./old-photo-pipeline $folder $i"
    ./old-photo-pipeline $folder $i

    echo "awk '(NR == 1) || (FNR > 1)' $folder/*.csv > $folder.csv"
    awk '(NR == 1) || (FNR > 1)' $folder/*.csv > $folder.csv

done
