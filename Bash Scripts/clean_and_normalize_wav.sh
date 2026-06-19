#!/bin/bash

# Ensure dependencies
if ! command -v aubiopitch &> /dev/null; then
    echo "Error: Dependency 'aubiopitch' could not be found. Please install it using: "
    echo "sudo pacman -S aubio"
    echo "or with the equivalent package manager for the environment."
    exit 1
fi
if ! command -v sox &> /dev/null; then
    echo "Error: Dependency 'sox' could not be found. Please install it using: "
    echo "sudo pacman -S sox"
    echo "or with the equivalent package manager for the environment."
    exit 1
fi

# User input for I/O directories, and parameters for sox command
read -p "Enter the SOURCE directory containing raw .wav files: " INPUT_DIR
read -p "Enter the TARGET directory for resultant files: " OUTPUT_DIR
read -p "Enter the peak amplitude relative to the digital ceiling (dB, normally negative to prevent clipping): " PEAK_AMPLITUDE
read -p "Enter the max gain clamp value (dB): " MAX_GAIN_CLAMP

if [ ! -d "$INPUT_DIR" ]; then
    echo "Error: Source directory '$INPUT_DIR' does not exist."
    exit 1
fi

ABS_INPUT=$(realpath "$INPUT_DIR" 2>/dev/null)
ABS_TARGET=$(realpath "$OUTPUT_DIR" 2>/dev/null)

IN_PLACE=false
if [ "$ABS_INPUT" = "$ABS_TARGET" ]; then
    IN_PLACE=true
    echo "Notice: Input and Output directories are the same. Files will be modified in-place."
fi

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
        echo "Cleaning old files from target directory..."
        find "$OUTPUT_DIR" -type f -iname "*.wav" -delete
    else
        mkdir -p "$OUTPUT_DIR"
    fi
fi

echo "Starting sample processing..."
INPUT_DIR="${INPUT_DIR%/}"
OUTPUT_DIR="${OUTPUT_DIR%/}"

find "$INPUT_DIR" -type f -iname "*.wav" -print0 | while IFS= read -r -d '' file; do

    relative_path="${file#$INPUT_DIR/}"
    output_file="$OUTPUT_DIR/$relative_path"

    # Determine the pitch using aubiopitch so can do dynamic trimming of wav files
    RAW_HZ=$(aubiopitch "$file" -p yinfft -B 2048 -s -45 -u Hz 2>/dev/null | awk '
        $2 > 0 {
            # Round the frequency to the nearest 5 Hz bin
            bin = int(($2 + 2.5) / 5) * 5;
            counts[bin]++;
            frames++;

            if (frames >= 30) {
                exit;
            }
        }
        END {
            max_count = 0;
            mode_bin = 0;
            for (b in counts) {
                if (counts[b] > max_count) {
                    max_count = counts[b];
                    mode_bin = b;
                }
            }
            print mode_bin;
        }'
    )

    HZ=$(printf "%.0f" "${RAW_HZ:-0}")

    duration="0.5"
    threshold="0.1%"
    octave_label="Unknown"

    if [ "$HZ" -gt 0 ]; then
        if [ "$HZ" -lt 131 ]; then
            # Notes up to B2 (frequencies <= 130.81 Hz)
            duration="0.3"
            threshold="0.4%"
            octave_label="Bass (C0-B2)"
        elif [ "$HZ" -lt 988 ]; then
            # Notes up to B5 (frequencies between 131 Hz and 987.77 Hz)
            duration="0.5"
            threshold="0.1%"
            octave_label="Midrange (C3-B5)"
        else
            # Notes from C6 and up (frequencies >= 988 Hz)
            duration="0.9"
            threshold="0.02%"
            octave_label="Treble (C6+)"
        fi
    fi

    echo "Analyzed: $relative_path [Detected: $HZ Hz, $octave_label ]"

    # Determine the exact linear multiplier needed to reach 0 dBFS
    MULT_TO_0=$(sox "$file" -n stat -v 2>&1)

    # Use AWK to safely calculate the bounded decibel gain without using limiters
    GAIN_DB=$(awk -v target_db="$PEAK_AMPLITUDE" -v mult="$MULT_TO_0" -v max_gain="$MAX_GAIN_CLAMP" 'BEGIN {
        # Linear multiplier to decibels
        gain_to_0_db = 20 * (log(mult) / log(10));

        # Add target headroom (e.g. -3 dB)
        req_gain = gain_to_0_db + target_db;

        # Apply safety clamp
        if (req_gain > max_gain) {
            printf "%.3f", max_gain;
        } else {
            printf "%.3f", req_gain;
        }
    }')

    SILENCE_FLAGS="silence 1 0.01 0.2% 1 $duration $threshold"

    # 3. Apply the front trim, back trim, and static safe gain in a single pass
    if [ "$IN_PLACE" = true ]; then
        temp_file="${file}.tmp.wav"
        sox "$file" "$temp_file" $SILENCE_FLAGS gain "$GAIN_DB"
        mv "$temp_file" "$file"
    else
        mkdir -p "$(dirname "$output_file")"
        sox "$file" "$output_file" $SILENCE_FLAGS gain "$GAIN_DB"
    fi

    echo "Processed: $relative_path [Gain: ${GAIN_DB}]"
done

echo "Success! .wav files saved in '$OUTPUT_DIR'"
exit 0