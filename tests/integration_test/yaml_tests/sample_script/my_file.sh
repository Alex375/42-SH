#!/bin/sh

while [ $# -ne $((0)) ]
do
    if [ -f "$1" ]
    then 
        echo "$1: file"
    elif [ -d "$1" ]
    then
        echo "$1: directory"
    else 
        echo "$1: unknown"
    fi
    shift
done