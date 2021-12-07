#!/bin/sh

if [ $# -ne $((2)) ]
then 
    echo "Usage: ./hcf.sh num1 num2"
    exit 1
fi
a=$1
b=$2

if [ "$b" -eq $((0)) ]
then
    echo "$a"
    exit 0
fi
if [ "$a" -eq $((0)) ]
then
    echo "$b"
    exit 0
fi

rem=$((a % b))


while [ $rem -ne $((0)) ];
do
    rem=$((a % b))
    a=$b
    b=$rem
done

echo "$a"