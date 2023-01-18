#!/bin/bash
filename=main.csv
if [ -f $filename ]; then
    echo "$filename already exists."
else
    touch main.csv
    echo "Date,Category,Amount,Name" >> main.csv
fi
echo "$1,$2,$3,$4" >> main.csv
echo "Inserted $1,$2,$3,$4 into main.csv"

while getopts ":c:n:s:h" opt; do
    case $opt in
    c)
        echo "option c was triggered, argument: $OPTARG"
        category=$OPTARG
        awk 'BEGIN {
            FS=","
            amount=0
        }
        {
            if($2 == category) {
                amount+=$3
            }
        }
        END {
            print amount
        }' 

        ;;
    n)
        name=$OPTARG
        ;;
    s)
        #sort csv by column name
        ;;
    h)
        echo "NAME\n\tq8.sh - a script to track expenses\nSYNOPSIS\n\tq8.sh [OPTION]... RECORD...\nDESCRIPTION\n\tCreate the csv file main.csv if it does not exist and insert the record into the file.\n\t-c, =Category: accepts a category and prints the amount of money spend in that category\n\t-n, =Name: accepts a name and print the amount of money spent by that person\n\t-s, =Column: accepts a column name sort the csv file by the column name\n\t-h, =help: show help prompt"
        ;;
    \?)
        echo "Invalid option: -$OPTARG" >&2
        ;;
    esac
done