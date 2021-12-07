

if [ $# -ne 1 ]
then
    echo "Sorry, expected 1 argument but $# were passed"
    exit 1
fi

FILENAME=$1

if [ -f "$FILENAME" ]
then 
    cat "$FILENAME"
else
    echo "$FILENAME:"
    printf "\tis not a valid file\n"
    exit 2
fi
