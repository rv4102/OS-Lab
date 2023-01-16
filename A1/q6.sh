#!/bin/bash
filename=input.txt
maxn=1000005
declare -a arr
for i in $(seq 2 $maxn)
do
    arr[$i]=1
done
i=2
while [ $i -le $maxn ]
do
    if [ ${arr[$i]} -eq 1 ]
    then
        mul=$(expr $i \* 2)
        while [ $mul -le $maxn ]
        do
            arr[$mul]=0
            mul=$(expr $mul + $i)
        done
    fi
    i=$(expr $i + 1)
done
output_path=output.txt
touch $output_path
while read line
do
    n=$line
    i=2
    while [ $i -le $n ]
    do
        if [ ${arr[$i]} -eq 1 ]
        then
            printf "%d " $i >> $output_path
        fi  
        i=$(expr $i + 1)
    done
    printf "\n" >> $output_path
done < $filename
