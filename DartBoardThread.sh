#serial processing
#!/bin/bash

echo "Dart Board Serial test in progress..."
echo "Number;Time" > DartBoardThread.csv

for i in {1..10}; do
    echo "Running iteration $i"
    for j in 100000 1000000 10000000 100000000 1000000000 10000000000 100000000000; do
        result=$(./DartBoardThread $j)
        echo "$j;$result" >> DartBoardThread.csv
    done
done

echo "" >> 'DartBoardThread.csv'
echo "done"
