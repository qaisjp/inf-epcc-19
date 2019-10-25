#!/bin/bash

# Colors
GREY='\033[0;37m' # Well, light grey.
RED='\033[1;31m' # It's actually light red
NC='\033[0m' # No Color
CYAN='\033[0;36m'
YELLOW='\033[1;33m'

export OMP_NUM_THREADS=4

for SCHED in "static" "auto" \
             "static,01" "static,02" "static,04" "static,08" "static,16" "static,32" "static,64" \
             "dynamic,01" "dynamic,02" "dynamic,04" "dynamic,08" "dynamic,16" "dynamic,32" "dynamic,64" \
             "guided,01" "guided,02" "guided,04" "guided,08" "guided,16" "guided,32" "guided,64"; do
    export OMP_SCHEDULE="$SCHED"
    for NUM in {00..99}; do
        FILENAME="cw1-$SCHED-$NUM"
        echo "i = ${NUM} - Schedule is: ${OMP_SCHEDULE}"

        if ! qsub loops.pbs -o "${FILENAME}.stdout.txt" -e "${FILENAME}.stderr.txt" -V; then
            echo -e "${RED}qsub was not happy!${NC}"
            exit 1
        fi
    done
done

# sleep 0.1s

# FILESIZE=$(stat -c%s "cw1.stderr.txt")
# if [ $FILESIZE -ne 0 ]; then
#     echo -e "${RED}**STDERR**${NC}"
#     cat "cw1.stderr.txt"
# fi

# echo -e "${CYAN}**STDOUT**${NC}"
# cat cw1.stdout.txt

# rm cw1.stdout.txt
# rm cw1.stderr.txt