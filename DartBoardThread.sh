#serial processing
#!/bin/bash

echo "Dart Board Thread test in progress..."
echo "Number;Threads;Time" > DartBoardThread.csv

for i in {1..10}; do
    echo "Running iteration $i"
    for t in 2 4 8 16 32; do
        for j in 100000 1000000 10000000 100000000 1000000000 10000000000; do
            result=$(./DartBoardThread $j $t)
            echo "$j;$t;$result" >> DartBoardThread.csv
        done
    done
done

echo "" >> 'DartBoardThread.csv'
echo "done"
