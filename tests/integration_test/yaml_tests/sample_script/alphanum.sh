
if [ "$#" -eq 0 ] ;then
    while read -r line; do 
        if [ "$(printf "%s" $line | grep -E "^$")" = "$line" ]; then
            echo "it is empty"
        elif [ "$(printf "%s" $line | grep -E "^[0-9]{1}$")" = "$line" ]; then
            echo "it is a digit"
        elif [ "$(printf "%s" $line | grep -E "^[a-zA-Z]+$")" = "$line" ]; then
            echo "it is a word"
        elif [ "$(printf "%s" $line | grep -E "^[0-9][0-9]+$")" = "$line" ]; then
            echo "it is a number"
        elif [ "$(printf "%s" $line | grep -E "^[a-zA-Z0-9]+$")" = "$line" ]; then
            echo "it is an alphanum"
        else 
            echo "it is too complicated"
            exit 0
        fi
    done
    exit 0
fi
exit 1