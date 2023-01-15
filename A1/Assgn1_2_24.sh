#!/bin/bash

while IFS= read -r username; do
    if [[ ${#username} -ge 5 ]] && [[ ${#username} -le 20 ]]; then
        if test -z "$(echo -n "$username" | tr -d 'a-zA-Z0-9')"; then
            if [[ $(echo "$username" |grep -o "[0-9]" |grep -c "") -ne 0 ]]; then
                if [[ $username =~ ^[a-zA-Z] ]]; then
                    # check if username contains a substring mentioned in list of words in $2
                    
                else 
                    echo "NO" 
                fi
            else 
                echo "NO" 
            fi
        else 
            echo "NO" 
        fi
    else 
        echo "NO" 
    fi
done < $1