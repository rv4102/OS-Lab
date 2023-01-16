#!/bin/bash
filename=$1
input_word=$2
while read line;do
    found=0
    echo $line | grep -q $input_word && found=1
    if [ $found -eq 1 ];then
        
    else
        echo $line
    fi
done < $filename