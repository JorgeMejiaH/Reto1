#serial processing
#!/bin/bash

echo "Monte Carlo Needle Thread test in progress..."
echo "Number;Threads;Time" > MonteCarloNeedleThread.csv

for i in {1..10}; do
    echo "Running iteration $i"
    for t in 2 4 8 16 32; do
        for j in 100000 1000000 10000000 100000000 1000000000 10000000000; do
            result=$(./MonteCarloNeedleThread $j $t)
            echo "$j;$t;$result" >> MonteCarloNeedleThread.csv
        done
    done
done

echo "" >> 'MonteCarloNeedleThread.csv'
echo "done"
