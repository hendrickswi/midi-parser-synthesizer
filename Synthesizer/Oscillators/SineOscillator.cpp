#include "SineOscillator.h"

#include <cmath>

SineOscillator::SineOscillator() {
    current_phase = 0;
    phase_increment = calculate_phase_increment(1.0, 44100.0);
}

SineOscillator::SineOscillator(double hz, double sample_rate) {
    current_phase = 0;
    phase_increment = calculate_phase_increment(hz, sample_rate);
}

SineOscillator::SineOscillator(const SineOscillator& other) {
    current_phase = other.current_phase;
    phase_increment = other.phase_increment;
}

double SineOscillator::get_sample() {
    current_phase = fmod(current_phase + phase_increment, TWO_PI);
    return std::sin(current_phase);
}

void SineOscillator::set_frequency(double hz, double sample_rate) {
    phase_increment = calculate_phase_increment(hz, sample_rate);
}