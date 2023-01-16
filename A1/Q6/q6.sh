#!/bin/bash
input_file=input1.txt
maxn=1000000
declare -a is_prime
sieve_of_eratosthene() {
    for((i=2; i<=maxn; i++))
    do
        is_prime[i]=1
    done
    for((p=2; p*p<=maxn; p++))
    do 
        if [[ ${is_prime[p]} -eq 1 ]]; then
            for((i=p*p; i<=maxn; i+=p))
            do
                is_prime[i]=0
            done   
        fi 
    done
}
sieve_of_eratosthene
output_path=output1.txt
while read -r n
do
    for((i=1; i<=n; i++))
    do
        if [[ ${is_prime[i]} -eq 1 ]]; then
            echo -n "$i "
        fi 
    done
    echo
done < $input_file > $output_path
