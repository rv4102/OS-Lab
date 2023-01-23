#!/bin/bash

for f in $(find "$1" -name '*.py'); do 
    echo "File: $f" # the file name and path are printed together

    line_num=1
    reading_multiline=0
    while read line; do
        # handle case where docstring is written in one line
        if [[ $line =~ ^[[:space:]]*\"\"\"[^\"]*\"\"\"$ ]]; then
            echo "$line_num: $line"
            ((line_num++))
            continue
        fi
        if [[ $line =~ ^[[:space:]]*\'\'\''[^\']*\'\'\'$ ]]; then
            echo "$line_num: $line"
            ((line_num++))
            continue
        fi

        if (( reading_multiline == 1 )); then
            echo "    $line"
        fi
        
        if [[ $line =~ ^[^\"]*[[:space:]]*\"\"\"[[:space:]]*$ ]] && (( reading_multiline == 1 )); then
            reading_multiline=0
        elif [[ $line =~ ^[[:space:]]*\"\"\".*[[:space:]]*$ ]] && (( reading_multiline == 0 )); then
            echo "$line_num: $line"
            reading_multiline=1
        fi

        if [[ $line =~ ^[^\']*[[:space:]]*\'\'\'[[:space:]]*$ ]] && (( reading_multiline == 1 )); then
            reading_multiline=0
        elif [[ $line =~ ^[[:space:]]*\'\'\'.*[[:space:]]*$ ]] && (( reading_multiline == 0 )); then
            echo "$line_num: $line"
            reading_multiline=1
        fi

        # this regex will accept both comments starting in a new line and comments which are present adjacent to a given code
        if [[ $line =~ ^[^#]*[[:space:]]*#.*$ ]]; then  
            echo "$line_num: $line"
        fi
        ((line_num++))
    done < $f
    echo ""
done
