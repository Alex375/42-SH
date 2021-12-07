#!/bin/sh

[ $# -ne $((1)) ] && echo "Sorry, expected 1 argument but $# were passed" && exit 1
FILENAME=$1
([ -f "$FILENAME" ] && cat "$FILENAME" && exit 0) || (echo "$FILENAME:" && printf "\tis not a valid file\n" && exit 2)
