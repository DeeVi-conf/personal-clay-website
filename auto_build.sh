#!/bin/bash

WATCH_DIR="."
BUILD_SCRIPT="./build.sh"

echo "Watching $WATCH_DIR for changes in *.c files..."
while true; do
    inotifywait -e close_write,create,delete,move "$WATCH_DIR"/*.c
    echo "Change detected, rebuilding..."
    bash "$BUILD_SCRIPT"
done
