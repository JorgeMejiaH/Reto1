#serial processing
#!/bin/bash

echo "Dart Board Serial test in progress..."
echo "Number;Time" > DartBoardSerial.csv

for i in {1..10}; do
    echo "Running iteration $i"
    for j in 100000 1000000 10000000 100000000 1000000000 10000000000 100000000000; do
        result=$(./DartBoardSerial $j)
        echo "$j;$result" >> DartBoardSerial.csv
    done
done

echo "" >> 'DartBoardSerial.csv'
echo "done"
