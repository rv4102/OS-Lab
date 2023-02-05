#!/bin/bash
CSV=main.csv
bold=$(tput bold)
normal=$(tput sgr0)
if [[ ! -f $CSV ]]
then
    touch $CSV
    echo "Date,Category,Amount,Name" >> $CSV
fi
function help(){
    echo -e "${bold}NAME${normal}\n\tq8.sh - a script to track expenses\n${bold}SYNOPSIS${normal}\n\tq8.sh [OPTION]... RECORD...\n${bold}DESCRIPTION\n${normal}\tCreate the csv file main.csv if it does not exist and inserts records into the file.\n\t${bold}-c, =Category\n\t\t${normal}accepts a category and prints the amount of money spend in that category\n\t${bold}-n, =Name${normal}\n\t\t accepts a name and print the amount of money spent by that person\n\t${bold}-s, =Column\n\t\t${normal} accepts a column name sort the csv file by the column name\n\t${bold}-h, =help\n\t\t${normal} show help prompt"
}
function AmountByCategory(){
    echo 
    tot_amt=0
    while IFS="," read -r _ category amount _
    do
        if [ "$category" == "$1" ]
        then
            tot_amt=$(($tot_amt + $amount))
        fi
    done < $CSV
    echo "Total Amount in Category : $1 is $tot_amt"
}

function AmountByName(){
    tot_amt=0
    while IFS="," read -r _ _ amount name
    do
        if [ "$name" == "$1" ]
        then
            tot_amt=$(($tot_amt + $amount))
        fi
    done < $CSV
    echo "Total Amount by : $1 is $tot_amt"
}
function SortByCol(){
    col=0
    case $1 in
    Date) col=1;;
    Category) col=2;;
    Amount) col=3;;
    Name) col=4;;
    *) echo "Invalid Column : $1 , the valid columns are : Date, Category, Amount, Name";;
    esac        
    if [ $col -eq 1 ]
    then
        out=$(tail -n +2 $CSV | sort -t- -k 3.1,3.2 -k 2n -k 1n)
    else
        out=$(tail -n +2 $CSV | sort -t, -k$col)
    fi
    echo "Date,Category,Amount,Name" > $CSV
    echo "$out" >> $CSV
}

declare -a flags
while getopts ":c:n:s:h" flag;do
    case $flag in
    c) category=$OPTARG flags+=($flag);;
    n) name=$OPTARG flags+=($flag);;    
    s) sort=$OPTARG flags+=($flag);;
    h) help=1 flags+=($flag);;
    ?)echo "Invalid Flag Recieved : $OPTARG";;
    esac
done

shift $((OPTIND-1))

if [[ $# -eq 4 ]]; then
    echo $1,$2,$3,$4 >> $CSV
    SortByCol "Date"
fi

for flag in ${flags[@]}
do
    case $flag in
    c) AmountByCategory "$category";;
    n) AmountByName "$name";;
    s) SortByCol "$sort";;
    h) help;;
    esac
done

