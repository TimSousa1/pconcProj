#!/bin/bash
# dir first argument

for folder in $1/*; do
for ((i=1; i<=$2; i*=2)); do
    rm -r $folder/old_photo_PAR_A
    echo running for $i threads
    echo ./old-photo-paralelo-A $folder $i
    ./old-photo-paralelo-A $folder $i
done;
done

awk '(NR == 1) || (FNR > 1)' *.csv > out.csv
rm timing*
