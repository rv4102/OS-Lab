#!/bin/bash

while read line; do
    if [[ $line =~ "$2" ]]; then
        echo "${line^^}" | sed -e:a -e 's/\([A-Z][^a-zA-Z]*\)\([A-Z]\)/\1\l\2/;ta'
    else
        echo "$line"
    fi
done < $1