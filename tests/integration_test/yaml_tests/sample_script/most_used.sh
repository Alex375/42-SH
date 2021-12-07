#!/bin/sh

cut -d " " -f 1 "$HOME/.bash_history" | sort -d \
| uniq -c | sort -nr | tr -s " " \
| head | cut -d " " -f2-
