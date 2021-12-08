echo $1; echo $2; echo $3;


for n in $@; do echo at : $n; done
echo
for n in "$@"; do echo at_quote : $n; done
echo
for n in "$@"; do echo at_quote_quote : "$n"; done
echo
for n in $*; do echo star : $n; done
echo
for n in "$*"; do echo star_quote : $n; done
echo
for n in "$*"; do echo star_quote_quote : "$n"; done
echo
