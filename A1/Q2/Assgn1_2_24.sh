#!/bin/bash

while IFS= read -r username; do
    flag=0
    if [[ ${#username} -ge 5 ]] && [[ ${#username} -le 20 ]]; then
        if test -z "$(echo -n "$username" | tr -d 'a-zA-Z0-9')"; then
            if [[ $(echo "$username" |grep -o "[0-9]" |grep -c "") -ne 0 ]]; then
                if [[ $username =~ ^[a-zA-Z] ]]; then
                    # check if username contains a substring mentioned in list of words in $2
                    while IFS= read -r fruitname; do
                        if [[ $(echo -n "$username" | grep -iFc "$fruitname") -ne 0 ]]; then
                            flag=1
                            break
                        fi
                    done < $2 
                else
                    flag=1
                fi
            else
                flag=1
            fi
        else
            flag=1
        fi
    else
        flag=1
    fi
    if [[ $flag -eq 1 ]]; then
        echo "NO"
    else
        echo "YES"
    fi
done < $1