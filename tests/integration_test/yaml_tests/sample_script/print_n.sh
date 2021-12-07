#!/bin/sh

if [ $# -eq 0 ]; then 
    exit 1
fi

NUM=$1
shift

if [ "$NUM" -lt $((0)) ]; then 
    exit 2
fi
for i in "$@"
do
    for _ in $(seq "$NUM")
    do 
        echo "$i"
    done
done
exit 0