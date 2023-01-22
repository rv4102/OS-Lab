#!/bin/bash
input_file=$1
maxn=1000000

declare -a spf
sieve_of_eratosthene() {
    for((i=1; i<=maxn; i++))
    do
        spf[i]=$i
    done
    for((i=4; i<=maxn; i+=2))
    do
        spf[i]=2
    done
    for((p=3; p*p<=maxn; p=p+2))
    do 
        if [[ ${spf[p]} -eq $p ]]; then
            for((i=p*p; i<=maxn; i+=p))
            do
                if [[ ${spf[i]} -eq $i ]]; then
                    spf[i]=$p
                fi
            done   
        fi 
    done
}
sieve_of_eratosthene
output_path=output.txt
while read -r n
do
    for((i=1; i<=n; i++))
    do
        while [ $n -ne 1 ]
        do
            factor=${spf[n]}
            while [[ $(($n%$factor)) -eq 0 ]]
            do
                n=$((n/factor))
            done
            echo -n "$factor "
        done
    done
    echo
done < $input_file > $output_path
