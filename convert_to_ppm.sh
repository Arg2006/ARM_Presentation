#!/bin/bash

TEST_JPEG_DIR="test_suite/images/input"
TEST_PPM_DIR="test_suite/images/ppm"
OUTPUT_FILE="test_suite/test_batch.h"
ROOT_IMAGE="leaf.jpg"

# Create PPM directory if it doesn't exist
mkdir -p "$TEST_PPM_DIR"

echo "Converting JPEGs to PPM..."

# Convert all JPEGs in the test directory
for img in "$TEST_JPEG_DIR"/*.{jpg,jpeg}; do
    [ -e "$img" ] || continue  # Skip if no match
    base=$(basename "$img")
    name="${base%.*}"
    convert "$img" "$TEST_PPM_DIR/$name.ppm"
    echo "Converted $img => $TEST_PPM_DIR/$name.ppm"
done

# Convert the root leaf.jpg file
if [ -f "$ROOT_IMAGE" ]; then
    name="${ROOT_IMAGE%.*}"
    convert "$ROOT_IMAGE" "$name.ppm"
    echo "Converted $ROOT_IMAGE => $name.ppm"
else
    echo "Warning: $ROOT_IMAGE not found in root directory"
fi

echo "Converted jpeg => ppm"
