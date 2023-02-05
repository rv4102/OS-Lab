#!/bin/bash
sed 's/.* //' $1|uniq -c|sed 's/\([0-9]\)/-\1/'|sort -t ' ' -k2|sort -t ' ' -nk1|awk '{printf "%s %d\n", $2, -$1}'
cat $1|cut -d" " -f1|sort|uniq -c|awk '{if($1==1) count++; else print $2} END {print count}'