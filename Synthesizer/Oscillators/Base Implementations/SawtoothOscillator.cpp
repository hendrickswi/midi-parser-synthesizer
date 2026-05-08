#include "SawtoothOscillator.h"
#include <cmath>

SawtoothOscillator::SawtoothOscillator() = default;

SawtoothOscillator::SawtoothOscillator(float hz, float sample_rate)
    : Oscillator(hz, sample_rate) {
}

SawtoothOscillator::SawtoothOscillator(const SawtoothOscillator& other) = default;

float SawtoothOscillator::get_sample() {
    current_phase += phase_increment;
    if (current_phase >= TWO_PI) {
        current_phase -= TWO_PI;
    }

    return current_phase;
}