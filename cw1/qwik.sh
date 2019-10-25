#!/bin/bash

# Colors
GREY='\033[0;37m' # Well, light grey.
RED='\033[1;31m' # It's actually light red
NC='\033[0m' # No Color
CYAN='\033[0;36m'
YELLOW='\033[1;33m'

make test
for THREADS in 01 02 04 06 08 12 16; do
    export THREADS
    echo ""
    echo ""
    for NUM in {00..99}; do
        export OMP_SCHEDULE="static,64"
        FILENAME="cw1-thread-$THREADS-n$NUM"
        echo "$FILENAME - sched: ${OMP_SCHEDULE}"
        if ! qsub threadloops.pbs -o "${FILENAME}.stdout.txt" -e "${FILENAME}.stderr.txt" -V; then
            echo -e "${RED}qsub was not happy!${NC}"
            exit 1
        fi
    done
    echo ""
    echo ""
done