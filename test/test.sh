#!/usr/bin/env bash
set -eu

for b in alchemy-write alchemy-read; do
    if [ ! -f $(dirname $0)/../build/${b} ]; then
        echo "${b} binary not found" && exit 1
    fi
done

writePBF="$(dirname $0)/../build/alchemy-write"
readPBF="$(dirname $0)/../build/alchemy-read"

echo "Converting sample.csv to sample.pbf"
if [ -f $(dirname $0)/sample.pbf ]; then
    rm $(dirname $0)/sample.pbf
fi
time cat $(dirname $0)/sample.csv | $writePBF $(dirname $0)/sample.pbf

if [ -f $(dirname $0)/sample.pbf ]; then
    filesize="$(ls -l $(dirname $0)/sample.pbf -h | awk '{ print $5 }')"
    echo "sample.pbf size: $filesize"
else
    echo "Writing failed" && exit 1
fi

echo "Converting sample.pbf back to csv"
if [ -f $(dirname $0)/back.csv ]; then
    rm $(dirname $0)/back.csv
fi

time $readPBF $(dirname $0)/sample.pbf $(dirname $0)/back.csv > /dev/null

if [ -f $(dirname $0)/back.csv ]; then
    diff $(dirname $0)/sample.csv $(dirname $0)/back.csv
    if [ $? -ne 0 ]; then
        echo "Roundtrip file back.csv differs from original sample.csv" && exit 1
    else
        echo "Roundtrip a+"
    fi
fi

