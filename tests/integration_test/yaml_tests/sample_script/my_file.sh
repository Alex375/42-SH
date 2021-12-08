for arg in "$@"
do
    if [ -f "$arg" ]
    then
        echo $arg: file
    elif [ -d "$arg" ]
    then
        echo $arg: directory
    else
        echo $arg: unknown
    fi
done
