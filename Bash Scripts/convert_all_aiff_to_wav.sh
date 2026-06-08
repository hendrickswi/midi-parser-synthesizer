#!/bin/bash

# Ensure dependencies
if ! command -v sox &> /dev/null; then
    echo "Error: Dependency 'sox' could not be found. Please install it using: "
    echo "sudo pacman -S sox"
    echo "or with the equivalent package manager for the environment."
    exit 1
fi

read -p "Enter the SOURCE directory containing raw .aiff files: " INPUT_DIR
read -p "Enter the TARGET directory for converted files: " OUTPUT_DIR

if [ ! -d "$INPUT_DIR" ]; then
    echo "Error: Source directory '$INPUT_DIR' does not exist."
    exit 1
fi

ABS_INPUT_PATH=$(realpath "$INPUT_DIR" 2>/dev/null)
ABS_OUTPUT_PATH=$(realpath "$OUTPUT_DIR" 2>/dev/null)

if [ "$ABS_INPUT_PATH" = "$ABS_OUTPUT_PATH" ]; then
    echo "Error: source and target directories are the same."
    echo "To preserve source assets, this operation is not allowed. Exiting..."
    exit 1
fi

# Do not allow operations directory on these critical directories
PROTECTED_DIRS=(
    "/" "/bin" "/boot" "/dev" "/etc" "/home" "/lib" "/lib64"
    "/mnt" "/opt" "/root" "/run" "/sbin" "/srv" "/sys" "/tmp"
    "/usr" "/var" "$HOME"
)
for protected in "${PROTECTED_DIRS[@]}"; do
    if [ "$ABS_OUTPUT_PATH" = "$protected" ]; then
        echo "Target directory resolves to a protected system path ($ABS_TARGET)!"
        echo "Operation aborted to prevent potential system damage."
        exit 1
    fi
done

if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir -p "$OUTPUT_DIR"
fi

echo "Starting .aiff to .wav conversion process..."
INPUT_DIR="${INPUT_DIR%/}"
OUTPUT_DIR="${OUTPUT_DIR%/}"

find "$INPUT_DIR" -type f \( -iname "*.aiff" -o -iname "*.aif" \) -print0 | while IFS= read -r -d '' file; do

    relative_path="${file#$INPUT_DIR/}"
    filename_no_extension="${relative_path%.*}"
    output_file="$OUTPUT_DIR/${filename_no_extension}.wav"
    mkdir -p "$(dirname "$output_file")"
    sox "$file" -b 16 -e signed-integer "$output_file"
    echo "Processed: $relative_path -> ${filename_no_extension}.wav"
done

echo "Success: .wav library available in '$OUTPUT_DIR'"
exit 0