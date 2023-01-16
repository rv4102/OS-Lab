#!/bin/bash
filename=$1
LCM=1
line_no=0
while read line;do
n=$line
num=0
line_no=$(expr $line_no + 1)
while [ $n -gt 0 ];do
    num=$(expr $num \* 10)
    k=$(expr $n % 10)
    num=$(expr $num + $k)
    n=$(expr $n / 10)
done
echo "Line = $line_no $num"
a=$LCM
b=$num
while [ $num -ne 0 ];do
    r=$(expr $LCM % $num)
    LCM=$num
    num=$r
done
p=$(expr $a / $LCM)
LCM=$(expr $p \* $b)
done < $filename
echo "LCM = $LCM"
