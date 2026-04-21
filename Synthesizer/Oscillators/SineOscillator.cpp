#include "SineOscillator.h"
#include <cmath>

SineOscillator::SineOscillator() = default;

SineOscillator::SineOscillator(double hz, double sample_rate)
    : Oscillator(hz, sample_rate) {
}

SineOscillator::SineOscillator(const SineOscillator& other) = default;

double SineOscillator::get_sample() {
    current_phase = fmod(current_phase + phase_increment, TWO_PI);
    return std::sin(current_phase);
}