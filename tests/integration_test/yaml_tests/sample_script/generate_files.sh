filename='default'
number=1
extension='txt'
next=""

for arg in "$@"
do
    if [ "$next" = "" ]
    then
        if [ "$arg" = "-f" ] || [ "$arg" = "--filename" ] 
        then
            next="file"
        elif [ "$arg" = "-n" ] || [ "$arg" = "--number" ] 
        then
            next="number"
        elif [ "$arg" = "-e" ] || [ "$arg" = "--extension" ] 
        then
            next="extension"
        else
            exit 1
        fi
    else
        if [ "$next" = "file" ]
        then
            filename=$arg
        elif [ "$next" = "extension" ]
        then
            extension=$arg
        elif [ "$next" = "number" ]
        then 
            number=$arg
        else
            exit 1
        fi
        next=""
    fi
done

