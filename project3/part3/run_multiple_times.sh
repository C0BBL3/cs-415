#!/bin/bash

# Number of times to run the command
num_runs=10000

# Command to run
command="./bank input-1.txt"

# Highly reccomend commenting out the check balance print line
# So u can see it working

# Loop to run the command multiple times
for ((i=1; i<=$num_runs; i++))
do
    echo "Run $i"
    $command
done