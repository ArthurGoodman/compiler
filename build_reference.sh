#!/bin/bash

gcc -c -o a.out $1
dump=`objdump -D -w`
rm a.out

echo "$dump" | while read line
do
    regex="^.*: ([0-9a-f ]*) (.*)$"
    [[ `echo $line` =~ $regex ]] && echo "${BASH_REMATCH[2]} ~ ${BASH_REMATCH[1]}"
done
