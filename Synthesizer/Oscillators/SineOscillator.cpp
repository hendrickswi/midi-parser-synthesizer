#include "SineOscillator.h"

#include <cmath>

constexpr double TWO_PI = 2 * 3.14159265358979323846;

inline double calculate_phase_increment(double hz, double sample_rate) {
    return TWO_PI * hz / sample_rate;
}

SineOscillator::SineOscillator() {
    current_phase = 0;
    phase_increment = calculate_phase_increment(1.0, 44100.0);
}

SineOscillator::SineOscillator(float hz, float sample_rate) {
    current_phase = 0;
    phase_increment = calculate_phase_increment(hz, sample_rate);
}

SineOscillator::SineOscillator(const SineOscillator& other) {
    current_phase = other.current_phase;
    phase_increment = other.phase_increment;
}

void SineOscillator::set_frequency(float hz, float sample_rate) {
    phase_increment = calculate_phase_increment(hz, sample_rate);
}

double SineOscillator::get_sample() {
    current_phase = fmod(current_phase + phase_increment, TWO_PI);
    return std::sin(current_phase);
}