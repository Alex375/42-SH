#!/bin/sh

if [ $# -eq $((0)) ]; then 
    exit 1
fi

if [ -d "$1" ]; then 
    mkdir "exe"
    mkdir "non-exe"
    for file in -- "$1"/*
    do 
        if [ -f "$file" ]; then
            if [ -x "$file" ]; then
                mv "$file" "exe/"
            else
                mv "$file" "non-exe/"
            fi
        fi
    done
exit 0
fi
exit 1
