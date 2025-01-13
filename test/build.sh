#!/bin/bash

# Clean up previous builds
rm -f process_one process_two process_three process_four process_five

# Build the processes with specific ports
g++ -o process_one   process.cpp -DLISTEN_PORT=6601 -DCONNECT_PORT=6605 -lpthread
g++ -o process_two   process.cpp -DLISTEN_PORT=6602 -DCONNECT_PORT=6601 -lpthread
g++ -o process_three process.cpp -DLISTEN_PORT=6603 -DCONNECT_PORT=6602 -lpthread
g++ -o process_four  process.cpp -DLISTEN_PORT=6604 -DCONNECT_PORT=6603 -lpthread
g++ -o process_five  process.cpp -DLISTEN_PORT=6605 -DCONNECT_PORT=6604 -lpthread

echo "Build complete: process_one, process_two, process_three, process_four, process_five"
