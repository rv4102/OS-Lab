#!/bin/bash
output_path=$2
input_path=$1
start=3
input_string="["
end=$(($#-1))
while [ $start -le $end ]
do
    input_string=$input_string"."$3","
    shift 1
    start=$(($start+1))
done
input_string=$input_string".$3]"
for f in $(find "$input_path" -name '*.jsonl'); do
    filename="$output_path$(basename "$f" | sed 's/.jsonl/.csv/')"
    echo "$input_string" | sed 's/\]//' | sed 's/\[//' | sed 's/\.//g' > $filename
    curl -s '$f' 
    cat $f | jq -r --arg input_string $input_string "$input_string | join(\",\")" >> $filename
done