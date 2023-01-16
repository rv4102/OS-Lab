#!/bin/bash
filename=$1
sed 's/.* //' $1 | uniq -c | sed 's/\([0-9]\)/-\1/' | sort -t ' ' -k2 | sort -t ' ' -nk1 | awk '{printf "%s %d\n", $2, -$1}'
awk '
{
    students[$1] += 1
}
END{
    single_course_stus = 0
    for (i in students){
        if (students[i] > 1){
            print i
        }
        if(students[i] == 1){
            single_course_stus += 1
        }
    }
    print single_course_stus
}' $filename
