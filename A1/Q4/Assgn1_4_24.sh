#!/bin/bash

while read line;do
    found=0
    echo $line | grep -q $2 && found=1
    if [ $found -eq 1 ];then
        echo "$line" | tr '[:lower:]' '[:upper:]' | sed -e:a -e 's/\([A-Z][^a-zA-Z]*\)\([A-Z]\)/\1\l\2/;ta'
    else
        echo "$line"
    fi
done < $1