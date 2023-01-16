#!/bin/bash
file_path=$1
out_path=/output/
if [ -z "$2" ] 
then
    out_path=output
else
    out_path=$2
fi
if [ ! -d "$out_path" ]
then
    mkdir $out_path
fi
for i in {a..z}
do
    touch $out_path/$i.txt
done
cd $file_path
for file in *.txt
do
    cat $file | while read line 
    do
        echo $line >> $out_path/${line:0:1}.txt
    done
done
for i in {a..z}
do
    sort -o $out_path/$i.txt $out_path/$i.txt 
done

