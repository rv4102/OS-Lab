#!/bin/bash

while IFS= read -r username; do
    # check the username
    if [[ ${#username} -ge 5 ]] && [[ ${#username} -le 20 ]]; then
        if test -z "$(echo -n "$username" | tr -d 'a-zA-Z0-9')"; then
            if [[ $(echo "$s" |grep -o "[0-9]" |grep -c "") -ne 0 ]]; then
                if [[ ${str::1} =~ [a-zA-Z] ]]; then
                    # check if username contains a substring mentioned in list of words in $2
                else echo "NO" fi
            else echo "NO" fi
        else echo "NO" fi
    else echo "NO" fi
done