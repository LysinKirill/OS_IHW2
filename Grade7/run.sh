#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <num_readers> <num_writers>"
    exit 1
fi

# Compile the program
gcc -o main main.c -lpthread

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful"
    # Run the program with provided arguments
    ./main "$1" "$2"
    # Delete the binary after execution
    rm main
else
    echo "Compilation failed"
fi
