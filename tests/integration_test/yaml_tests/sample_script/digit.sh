#!/bin/sh
while [ $# -ne $((0)) ]
do
    case "$1" in
        -[1-9]) 
            echo "$1"  is a negative digit.
            ;;
        [[:digit:]] | +[[:digit:]] )
            echo "$1"  is a positive digit.
            ;;
        *) 
            echo "$1"  is not a digit.
            ;;
    esac
    shift
done