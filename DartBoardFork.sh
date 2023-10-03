#serial processing
#!/bin/bash

echo "Dart Board process test in progress..."
echo "Number;Process;Time" > DartBoardFork.csv

for i in {1..10}; do
    echo "Running iteration $i"
    for p in 2 4 8 16 32; do
        for j in 100000 1000000 10000000 100000000 1000000000 10000000000; do
            result=$(./DartBoardFork $j $p)
            echo "$j;$p;$result" >> DartBoardFork.csv
        done
    done
done

echo "" >> 'DartBoardFork.csv'
echo "done"
