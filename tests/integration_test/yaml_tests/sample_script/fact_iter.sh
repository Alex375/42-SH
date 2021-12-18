if [ "$#" != 1 ]
then
    exit 1
fi


temp=1
i=1
while [ "$i" -le "$1" ]
do
    temp=$(( temp*i ))
    i=$((i+1))
done
echo $temp
exit 0

