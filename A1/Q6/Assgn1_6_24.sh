#!/bin/bash
maxn=1000000
declare -a spf
for ((i=1; i<=maxn; i++))
do
    spf[$i]=$i
done
for((i=2; i*i<=maxn; i++))
do
    if [ ${spf[$i]} -eq $i ]
    then
        for((j=i*i; j<=maxn; j+=i))
        do
            if [ ${spf[$j]} -eq $j ]
            then
                spf[$j]=$i
            fi
        done
    fi
done
>output.txt
while read n; do
    while [[ $n -ne 1 ]]; do
        echo -n "${spf[$n]} "
        n=$((n/${spf[$n]}))
    done
    echo
done < $1
