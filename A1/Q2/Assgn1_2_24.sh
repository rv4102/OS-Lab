#!/bin/bash
while read  username; do
    out=NO
    if echo ${username} | grep -qP "^[a-zA-Z](?=.*[0-9])[a-zA-Z0-9]{4,19}$" &&  ! grep -qif fruits.txt <<< ${username} ;then
        out=YES   
    fi
    echo $out >> "validation_results.txt"
done < $1
