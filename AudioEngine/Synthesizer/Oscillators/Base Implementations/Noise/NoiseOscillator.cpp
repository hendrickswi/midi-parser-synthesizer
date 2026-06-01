#include "NoiseOscillator.h"

#include <cstdlib>

NoiseOscillator::NoiseOscillator() = default;

NoiseOscillator::NoiseOscillator(const NoiseOscillator& other) = default;

void NoiseOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    for (int i = 0; i < num_frames; i++) {
        float random_val = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        buffer[i] = random_val * 2.0f - 1.0f;
    }
}

void NoiseOscillator::set_modulation_depth(float depth) {
}

void NoiseOscillator::set_frequency(float hz, float sample_rate) {
}