#!/bin/bash
# usage ./run_all_test.sh <Milestone (int)> <Argument (string)>
# Running test for milestone 3 with "decorated" argument: ./run_all_test.sh 3 decorated

TEST="./test"
BIN=""
COMPILER="./compiler"
MILESTONE=$1
ARGS="--$2"
INPUT_DIR="$TEST/milestone-$MILESTONE/input"
OUTPUT_DIR="$TEST/milestone-$MILESTONE/output"

echo "Running all milestone $MILESTONE test case"
make clean
make build

mkdir -p "$OUTPUT_DIR"

if [ ! -f "$COMPILER" ]; then
    echo "Error: build failed $COMPILER not found"
    exit 1
fi

for input_file in "$INPUT_DIR"/*.pas; do
    filename=$(basename "$input_file")
    name="${filename%.pas}"

    output_file="$OUTPUT_DIR/${name}.txt"

    echo ""
    echo "Testing: $filename"

    if "$COMPILER" "$input_file" "$ARGS" > "$output_file" 2>&1; then
        echo "Status: Success"
    else
        echo "Status: Failed"
    fi
done

exit 0