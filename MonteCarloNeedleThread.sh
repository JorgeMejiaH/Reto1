#serial processing
#!/bin/bash

echo "Monte Carlo Needle Serial test in progress..."
echo "Number;Time" > MonteCarloNeedleThread.csv

for i in {1..10}; do
    echo "Running iteration $i"
    for j in 100000 1000000 10000000 100000000 1000000000 10000000000 100000000000; do
        result=$(./MonteCarloNeedleThread $j)
        echo "$j;$result" >> MonteCarloNeedleThread.csv
    done
done

echo "" >> 'MonteCarloNeedleThread.csv'
echo "done"
