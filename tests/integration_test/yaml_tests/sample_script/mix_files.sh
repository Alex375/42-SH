#!/bin/sh

if [ "$#" -ne 2 ]; then
    >&2 echo "Usage: mix_files file1 file2"
    exit 1
fi

len1=$(wc -l < "$1")
len2=$(wc -l < "$2")

i1=0
i2=0


while [ $((i1)) -lt $((len1)) ] || [ $((i2)) -lt $((len2)) ]
do
        read -r line1
        read -r line2 <&3
        if [ "$i1" -lt "$len1" ]; then
            printf "%s\n" "$line1"
        fi
        if [ "$i2" -lt "$len2" ]; then
            printf "%s\n" "$line2"
        fi
        i1=$((i1 + 1))
        i2=$((i2 + 1))
done <$1 3<$2

exit 0