#!/bin/bash

while read -r username; do
    out=NO
    if (( 5 <= ${#username} <= 20 )) && [[ $username =~ ^[a-zA-Z][a-zA-Z0-9]*[0-9][a-zA-Z0-9]*$ ]]; then
        out=YES
        while read -r fruitname; do
            if [[ "${username,,}" == *"${fruitname,,}"* ]]; then
                out=NO
                break
            fi
        done < "fruits.txt"
    fi
    echo $out >> "validation_results.txt"
done < $1

