#!/bin/bash

sed 's/.* //' $1 | uniq -c | sed 's/\([0-9]\)/-\1/' | sort -t ' ' -k2 | sort -t ' ' -nk1 | awk '{printf "%s %d\n", $2, -$1}'