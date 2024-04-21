#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <test_number>"
    exit 1
fi

# Path to the directory containing test files
TEST_DIR="../Tests"
TEST_NUM="$1"
TEST_FILE="$TEST_DIR/$TEST_NUM.txt"

# Check if the test file exists
if [ ! -f "$TEST_FILE" ]; then
    echo "Test file $TEST_FILE not found"
    exit 1
fi

# Compile the program
gcc -o main main.c -lpthread

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful"
    
    # Run the program with the test file argument
    ./main $(cat "$TEST_FILE" | tr '\n' ' ')

    # Delete the binary after execution
    rm main
else
    echo "Compilation failed"
fi