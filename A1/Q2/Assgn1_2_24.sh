#!/bin/bash

while IFS= read -r username; do
    flag=0
    if [[ ${#username} -ge 5 ]] && [[ ${#username} -le 20 ]] && [[ $username =~ ^[a-zA-Z][a-zA-Z0-9]*[0-9][a-zA-Z0-9]*$ ]]; then
        while IFS= read -r fruitname; do
            if [[ $(echo -n "$username" | grep -iFc "$fruitname") -ne 0 ]]; then
                flag=1
                break
            fi
        done < $2 
    else
        flag=1
    fi
    if [[ $flag -eq 1 ]]; then
        echo "NO"
    else
        echo "YES"
    fi
done < $1