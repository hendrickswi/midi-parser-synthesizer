#include "NoiseOscillator.h"

#include <cstdlib>

NoiseOscillator::NoiseOscillator() = default;

NoiseOscillator::NoiseOscillator(const NoiseOscillator& other) = default;

float NoiseOscillator::get_sample() {
    float random_val = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return random_val * 2.0f - 1.0f;
}

void NoiseOscillator::set_modulation_depth(float depth) {
}

void NoiseOscillator::set_frequency(float hz, float sample_rate) {
}