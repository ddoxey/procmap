#!/bin/bash

# Define log directory and process names
LOG_DIR="./logs"
PROCESS_NAMES=("one" "two" "three" "four" "five")

# Ensure an argument is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 {start|status|stop}"
    exit 1
fi

# Handle the argument
case $1 in
    start)
        # Create log directory
        rm -rf "$LOG_DIR"
        mkdir -p "$LOG_DIR"
        echo "Starting processes..."
        
        # Start each process
        for process in "${PROCESS_NAMES[@]}"; do
            ./process_${process} > "$LOG_DIR/process_${process}.log" 2>&1 &
            echo "Started process_${process} (PID: $!) - Logging to $LOG_DIR/process_${process}.log"
        done
        echo "All processes started."
        ;;
    
    status)
        echo "Checking process statuses..."
        for process in "${PROCESS_NAMES[@]}"; do
            pid=$(pgrep -f "process_${process}")
            if [ -n "$pid" ]; then
                echo "process_${process} is running (PID: $pid)."
            else
                echo "process_${process} is not running."
            fi
        done
        ;;
    
    stop)
        echo "Stopping processes..."
        for process in "${PROCESS_NAMES[@]}"; do
            pid=$(pgrep -f "process_${process}")
            if [ -n "$pid" ]; then
                kill "$pid"
                echo "Stopped process_${process} (PID: $pid)."
            else
                echo "process_${process} is not running."
            fi
        done
        echo "All processes stopped."
        ;;
    
    *)
        echo "Invalid argument: $1"
        echo "Usage: $0 {start|status|stop}"
        exit 1
        ;;
esac
