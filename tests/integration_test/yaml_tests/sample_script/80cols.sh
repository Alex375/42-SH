if [ $# -ne 1 ]; then
    exit 1
fi

if [ -r "$1" ]; then
    while IFS= read -r line; do 
        char=$(echo -n "$line" | wc -m)
        if [ "$char" -ge 80 ]; then
            printf "%s\n" "$line"
        fi
    done <$1
    exit 0
fi
exit 1