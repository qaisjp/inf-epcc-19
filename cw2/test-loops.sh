#!/bin/bash

# Colors
GREY='\033[0;37m' # Well, light grey.
RED='\033[1;31m' # It's actually light red
NC='\033[0m' # No Color
CYAN='\033[0;36m'

OUTPUT_FILE=`mktemp`
./loops2 > $OUTPUT_FILE

SUCCESS_OK=1
if ! grep -q "Loop 1 check: Sum of a is -343021.474766" $OUTPUT_FILE; then
    echo -e "${GREY}FAILED LOOP 1:${NC}"
    grep "Loop 1 check: Sum of a is" $OUTPUT_FILE
    SUCCESS_OK=0
fi

if ! grep -q "Loop 2 check: Sum of c is -25242644.603147" $OUTPUT_FILE; then
    echo -e "${GREY}FAILED LOOP 2:${NC}"
    grep "Loop 2 check: Sum of c is" $OUTPUT_FILE
    SUCCESS_OK=0
fi

if [ $SUCCESS_OK -eq "0" ]; then
    echo -e "${RED}FAILED${NC}"
else
    echo -e "${CYAN}SUCCESS${NC}"
fi

echo ""
cat $OUTPUT_FILE
rm $OUTPUT_FILE