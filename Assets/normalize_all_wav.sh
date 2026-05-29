#!/bin/bash

read -p "Enter the SOURCE directory containing raw .wav files: " INPUT_DIR
read -p "Enter the TARGET directory for normalized files: " OUTPUT_DIR
read -p "Enter the peak amplitude relative to the digital ceiling: " AMT

if [ ! -d "$INPUT_DIR" ]; then
    echo "Error: Source directory '$INPUT_DIR' does not exist."
    exit 1
fi

IN_PLACE=false
if [ "$INPUT_DIR" -ef "$OUTPUT_DIR" ]; then
    IN_PLACE=true
    echo "Notice: Input and Output directories are the same. Files will be modified in-place."
fi

# Do not allow operations directory on these critical directories
PROTECTED_DIRS=(
    "/" "/bin" "/boot" "/dev" "/etc" "/home" "/lib" "/lib64"
    "/mnt" "/opt" "/root" "/run" "/sbin" "/srv" "/sys" "/tmp"
    "/usr" "/var" "$HOME"
)
for protected in "${PROTECTED_DIRS[@]}"; do
    if [ "$ABS_TARGET" = "$protected" ]; then
        echo "Target directory resolves to a protected system path ($ABS_TARGET)!"
        echo "Operation aborted to prevent potential system damage."
        exit 1
    fi
done

if [ "$IN_PLACE" = "false" ]; then
  if [ -d "$OUTPUT_DIR" ]; then
      echo "Syncing: Removing old .wav files from '$OUTPUT_DIR'..."
      find "$OUTPUT_DIR" -type f -iname "*.wav" -delete
  else
      mkdir -p "$OUTPUT_DIR"
  fi
fi

echo "Starting normalization process..."
INPUT_DIR="${INPUT_DIR%/}"
OUTPUT_DIR="${OUTPUT_DIR%/}"

find "$INPUT_DIR" -type f -iname "*.wav" -print0 | while IFS= read -r -d '' file; do

    relative_path="${file#$INPUT_DIR/}"
    output_file="$OUTPUT_DIR/$relative_path"

    if [ "$IN_PLACE" = true ]; then
        temp_file="${file}.tmp.wav"
        sox "$file" "$temp_file" norm "$AMT"
        mv "$temp_file" "$file"
    else
        mkdir -p "$(dirname "$output_file")"
        sox "$file" "$output_file" norm "$AMT"
    fi
    echo "Processed: $relative_path"
done

echo "Normalized library preserved in '$OUTPUT_DIR'"
exit 0