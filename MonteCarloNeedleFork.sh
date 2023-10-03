#serial processing
#!/bin/bash

echo "Monte Carlo Needle Process test in progress..."
echo "Number;Process;Time" > MonteCarloNeedleFork.csv

for i in {1..10}; do
    echo "Running iteration $i"
    for p in 2 4 8 16 32; do
        for j in 100000 1000000 10000000 100000000 1000000000 10000000000; do
            result=$(./MonteCarloNeedleFork $j $p)
            echo "$j;$p;$result" >> MonteCarloNeedleFork.csv
        done
    done
done

echo "" >> 'MonteCarloNeedleFork.csv'
echo "done"
