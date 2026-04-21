#include "SawtoothOscillator.h"

#include <cmath>
#include <math.h>

SawtoothOscillator::SawtoothOscillator() {
    current_phase = 0;
    phase_increment = calculate_phase_increment(1.0, 44100.0);
}

SawtoothOscillator::SawtoothOscillator(double hz, double sample_rate) {
    current_phase = 0;
    phase_increment = calculate_phase_increment(hz, sample_rate);
}

SawtoothOscillator::SawtoothOscillator(const SawtoothOscillator& other) {
    current_phase = other.current_phase;
    phase_increment = other.phase_increment;
}

double SawtoothOscillator::get_sample() {
    current_phase = fmod(current_phase + phase_increment, TWO_PI);
    return fmod(current_phase, TWO_PI);
}

void SawtoothOscillator::set_frequency(double hz, double sample_rate) {
    phase_increment = calculate_phase_increment(hz, sample_rate);
}