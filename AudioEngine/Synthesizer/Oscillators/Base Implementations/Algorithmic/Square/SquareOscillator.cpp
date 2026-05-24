#include "SquareOscillator.h"
#include <cmath>

inline int sign(const float val) {
    return (0.0 < val) - (val < 0.0);
}

SquareOscillator::SquareOscillator() = default;

SquareOscillator::SquareOscillator(float hz, float sample_rate)
    : AlgorithmicOscillator(hz, sample_rate) {
}

SquareOscillator::SquareOscillator(const SquareOscillator& other) = default;

float SquareOscillator::get_sample() {
    current_phase += phase_increment;
    if (current_phase >= TWO_PI) {
        current_phase -= TWO_PI;
    }

    return sign(std::sin(current_phase));
}