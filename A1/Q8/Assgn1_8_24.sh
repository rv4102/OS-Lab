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
    tot_amt=0
    while IFS="," read -r _ category amount _
    do
        if [ "$category" == "$OPTARG" ]
        then
            tot_amt=$(($tot_amt + $amount))
        fi
    done < $CSV
    # done < <( tail -n +2 $CSV )
    echo "Total Amount in Category : $OPTARG is $tot_amt"
}
function AmountByName(){
    tot_amt=0
    while IFS="," read -r _ _ amount name
    do
        if [ "$name" == "$OPTARG" ]
        then
            tot_amt=$(($tot_amt + $amount))
        fi
    done < $CSV
    # done < <( tail -n +2 $CSV )
    echo "Total Amount by : $OPTARG is $tot_amt"
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
    sort -t, -k$col $CSV -o $CSV     
}
while getopts ":c:n:s:h" flag;do
    case $flag in
    c) AmountByCategory "$OPTARG";;
    n) AmountByName "$OPTARG";;
    s) SortByCol "$OPTARG";;
    h) help ;;
    ?)echo "Invalid Flag Recieved : $OPTARG";;
    esac
done
shift $((OPTIND-1))


if [[ $# -eq 4 ]]; then
    echo $1,$2,$3,$4 >> $CSV
    SortByCol "Date"
fi
