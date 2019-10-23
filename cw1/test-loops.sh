#!/bin/bash
OUTPUT_FILE=`mktemp`
./loops > $OUTPUT_FILE
echo $OUTPUT_FILE
SUCCESS_OK=1
if ! grep "Loop 1 check: Sum of a is -343021.474766" $OUTPUT_FILE; then
    echo "FAILED LOOP 1:"
    grep "Loop 1 check: Sum of a is" $OUTPUT_FILE
    SUCCESS_OK=0
fi

if ! grep "Loop 2 check: Sum of c is -25242644.603147" $OUTPUT_FILE; then
    echo "FAILED LOOP 2:"
    grep "Loop 2 check: Sum of c is" $OUTPUT_FILE
    SUCCESS_OK=0
fi

if [ $SUCCESS_OK -eq "0" ]; then
    echo "FAILED"
else
    echo "SUCCESS"
fi

rm $OUTPUT_FILE