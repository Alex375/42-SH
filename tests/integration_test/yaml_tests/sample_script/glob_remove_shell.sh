
EXT="txt"
if [ $# -eq 1 ] && [ "$1" != "" ]
then
    EXT=$1
fi

rm -f ./*."$EXT"