#!/bin/bash
if [ ! -d "$2" ]
then
    mkdir $2
fi
for i in {a..z}
do
    touch $2/$i.txt
done
for file in $1/*.txt
do
    awk '{print>>"./'$2'/"tolower(substr($1,1,1))".txt"}' ./$1/$file
done
for i in {a..z}
do
    sort -o $2/$i.txt $2/$i.txt 
done