#!/bin/bash

filename=$1

while read line; do
# reading each line
./main $line 6
done < $filename