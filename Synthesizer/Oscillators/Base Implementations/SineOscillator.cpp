#include "SineOscillator.h"
#include <cmath>

SineOscillator::SineOscillator() = default;

SineOscillator::SineOscillator(float hz, float sample_rate)
    : Oscillator(hz, sample_rate) {
}

SineOscillator::SineOscillator(const SineOscillator& other) = default;

float SineOscillator::get_sample() {
    current_phase += phase_increment;
    if (current_phase >= TWO_PI) {
        current_phase -= TWO_PI;
    }

    return std::sin(current_phase);
}