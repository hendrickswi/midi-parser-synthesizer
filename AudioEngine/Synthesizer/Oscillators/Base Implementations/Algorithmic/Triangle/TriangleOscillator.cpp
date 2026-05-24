#include "TriangleOscillator.h"

#include <cmath>

TriangleOscillator::TriangleOscillator() = default;

TriangleOscillator::TriangleOscillator(float hz, float sample_rate)
    : AlgorithmicOscillator(hz, sample_rate) {
}

TriangleOscillator::TriangleOscillator(const TriangleOscillator& other) = default;

float TriangleOscillator::get_sample() {
    current_phase += phase_increment;
    if (current_phase >= TWO_PI) {
        current_phase -= TWO_PI;
    }

    float normalized = current_phase / TWO_PI;
    return 4.0f * std::abs(normalized - 0.5f) - 1.0f;
}