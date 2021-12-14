for arg in "$@"
do
    if [ "$arg" -eq "$arg" ] 2>/dev/null; then
        if [ "$arg" -lt 0 ]; then
            echo $arg is a negative digit.
        else
            echo $arg is a positive digit.
        fi
    else
        echo $arg is not a digit.
    fi
done
