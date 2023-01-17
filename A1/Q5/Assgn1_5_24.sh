#!/bin/bash

for f in $(find "$1" -name '*.py'); do 
    file=$(echo "$f"|tr "/" "\n"|tail -1)
    echo "\nName: $file \nPath: $f"
    grep -Eon '^ *#.*' $f
    sed -n '/"""/,/"""/p' $f
    sed -n "/'''/,/'''/p" $f
done
