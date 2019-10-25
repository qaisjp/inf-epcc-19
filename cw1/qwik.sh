#!/bin/bash

for n in 1 2 4 8 16 32 64; do
    echo ""
    echo ""
    export OMP_SCHEDULE="$1,$n"
    export OMP_NUM_THREADS=4
    echo $OMP_SCHEDULE
    make deploy
    echo ""
    echo ""
done