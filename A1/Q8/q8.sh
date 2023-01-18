#!/bin/bash
filename=main.csv
if [ ! -f $filename ]; then
    touch main.csv
fi
while getopts ":c:n:s:h" opt; do
    case $opt in
    c)
        awk -v category=$OPTARG 'BEGIN {
            FS=","
            amount=0
        }
        {
            if($2 == category) {
                amount+=$3
            }
        }
        END {
            print "Total amount spent in " category " is " amount
        }' main.csv
        ;;
    n)  
        awk -v name=$OPTARG 'BEGIN {
            FS=","
            amount=0
        }
        {
            if($4 == name) {
                amount+=$3
            }
        }
        END {
            print "Total amount spent by " name " is " amount
        }' main.csv
        ;;
    s)
        col=1
        case $OPTARG in
        Date)
            col=1
            ;;
        Category)
            col=2
            ;;
        Amount)
            col=3
            ;;
        Name)
            col=4
            ;;
        esac
        sort -t, -k$col main.csv -o main.csv 
        ;;
    h)
        echo "NAME\n\tq8.sh - a script to track expenses\nSYNOPSIS\n\tq8.sh [OPTION]... RECORD...\nDESCRIPTION\n\tCreate the csv file main.csv if it does not exist and insert the record into the file.\n\t-c, =Category: accepts a category and prints the amount of money spend in that category\n\t-n, =Name: accepts a name and print the amount of money spent by that person\n\t-s, =Column: accepts a column name sort the csv file by the column name\n\t-h, =help: show help prompt"
        ;;
    \?)
        echo "Invalid option: -$OPTARG" 
        ;;
    esac
done

shift $((OPTIND-1))
if [ $# -eq 4 ]; then
    echo $1,$2,$3,$4 >> main.csv
fi
