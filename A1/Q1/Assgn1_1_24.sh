# #!/bin/bash
lcm_res=1
rev $1|while read num; do
    a=$lcm_res
    b=$num
    while [ $num -ne 0 ]; do
        r=$((lcm_res%num))
        lcm_res=$num
        num=$r
    done
    lcm_res=$((a/lcm_res*b))
    echo $lcm_res
done|tail -1
