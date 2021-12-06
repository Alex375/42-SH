if [ $# -eq 0 ]
then
    exit 1
fi

if [ $1 -lt 0 ]
then
    exit 2
fi

pass=0

for arg in "$@"
do
    if [ $pass -eq 1 ]
    then
        for i in 1 2 3
        do
            echo $arg
        done
    else
        pass=1
    fi
done

exit 0


