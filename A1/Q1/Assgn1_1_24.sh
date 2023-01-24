# #!/bin/bash
numbers=$(cat $1 | while read num; do echo $num | rev; done)
lcm_res=1
for num in $numbers; do
    a=$lcm_res
    b=$num
    while [ $num -ne 0 ]; do
        r=$((lcm_res%num))
        lcm_res=$num
        num=$r
    done
    lcm_res=$((a/lcm_res*b))
done
echo "LCM=$lcm_res"
