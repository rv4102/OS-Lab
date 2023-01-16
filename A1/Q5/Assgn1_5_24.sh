#!/bin/bash

for f in $(find "$1" -name '*.py'); do 
    file=echo $f|tr "/" "\n"|tail -1
    echo "Name: $file"
    echo "Path: $f"
    
done
