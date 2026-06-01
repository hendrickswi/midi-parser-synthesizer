#include "TriangleOscillator.h"

#include <cmath>

TriangleOscillator::TriangleOscillator() = default;

TriangleOscillator::TriangleOscillator(float hz, float sample_rate)
    : AlgorithmicOscillator(hz, sample_rate) {
}

TriangleOscillator::TriangleOscillator(const TriangleOscillator& other) = default;

void TriangleOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    for (int i = 0; i < num_frames; i++) {
        float new_phase_increment = phase_increment;
        if (fm_buffer != nullptr) {
            float current_hz = base_hz + fm_buffer[i];
            new_phase_increment = calculate_phase_increment(current_hz, sample_rate);
        }

        float normalized_phase = current_phase / TWO_PI;
        buffer[i] = 4.0f * std::abs(normalized_phase - 0.5f) - 1.0f;

        current_phase += new_phase_increment;
        if (current_phase >= TWO_PI) {
            current_phase -= TWO_PI;
        }
    }
}