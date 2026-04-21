#include "Oscillator.h"

Oscillator::Oscillator() {
    current_phase = 0;
    phase_increment = calculate_phase_increment(1.0, 44100.0);
}

Oscillator::Oscillator(double hz, double sample_rate) {
    current_phase = 0;
    phase_increment = calculate_phase_increment(hz, sample_rate);
}

Oscillator::Oscillator(const Oscillator& other) {
    current_phase = other.current_phase;
    phase_increment = other.phase_increment;
}

Oscillator::~Oscillator() = default;

void Oscillator::set_frequency(double hz, double sample_rate) {
    phase_increment = calculate_phase_increment(hz, sample_rate);
}