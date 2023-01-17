#!/bin/bash

for f in $(find "$1" -name '*.py'); do 
    file=$(echo "$f"|tr "/" "\n"|tail -1)
    echo "\nName: $file"
    echo "Path: $f"
    grep -Eon '^ *#.*' $f
    grep -Eon '^ *""".*?' $f
done
