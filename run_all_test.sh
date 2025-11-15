#!/bin/bash

TEST="./test"
BIN=""
PASLEX="./paslex"
MILESTONE=2
INPUT_DIR="$TEST/milestone-$MILESTONE/input"
OUTPUT_DIR="$TEST/milestone-$MILESTONE/output"

echo "Running all milestone $MILESTONE test case"
make clean
make build

mkdir -p "$OUTPUT_DIR"

if [ ! -f "$PASLEX" ]; then
    echo "Error: build failed $PASLEX not found"
    exit 1
fi

for input_file in "$INPUT_DIR"/*.pas; do
    filename=$(basename "$input_file")
    name="${filename%.pas}"

    output_file="$OUTPUT_DIR/${name}.txt"

    echo ""
    echo "Testing: $filename"

    if "$PASLEX" "$input_file" > "$output_file" 2>&1; then
        echo "Status: Success"
    else
        echo "Status: Failed"
    fi
done

exit 0