#!/bin/bash
N=2000000000
echo "=== SERIAL ==="
./pi_s $N
echo ""
for T in 1 2 4 8 12; do
    echo "=== PARALLEL T=$T ==="
    ./pi_p $N $T
    echo ""
done
