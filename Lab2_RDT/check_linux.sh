#!/bin/bash

project='TCP'

for v in {1..10}
do
    echo $v
    ./$project/bin/$project > $v.tmp
    diff -s ./data/input.txt ./data/output.txt
    if [ $? -ne 0 ]
    then
        echo "Case $v Go wrong!"
        break
    fi
done
echo "All right!"