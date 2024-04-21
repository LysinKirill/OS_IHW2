#!/bin/bash

# Compile all programs
gcc -o reader reader.c -Wall -Wextra
gcc -o writer writer.c -Wall -Wextra
gcc -o init_memory init_memory.c -Wall -Wextra

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

# Initialize shared memory
./init_memory

# Function to run reader processes
run_readers() {
    local num_readers=$1
    for ((i = 1; i <= num_readers; i++)); do
        ./reader > "reader_$i.log" &
    done
}

# Function to run writer processes
run_writers() {
    local num_writers=$1
    for ((i = 1; i <= num_writers; i++)); do
        ./writer > "writer_$i.log" &
    done
}

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

# Read number of readers and writers from the test file
read num_readers num_writers < "$TEST_FILE"

# Run reader and writer processes
run_readers $num_readers
run_writers $num_writers

# Wait for all child processes to finish
wait

# Clean up compiled programs
rm reader writer init_memory

echo "Testing complete"
