#include "SquareOscillator.h"
#include "Oscillator.h"
#include <cmath>

inline int sign(const double val) {
    return (0.0 < val) - (val < 0.0);
}

SquareOscillator::SquareOscillator() {
    current_phase = 0;
    phase_increment = calculate_phase_increment(1.0, 44100.0);
}

SquareOscillator::SquareOscillator(double hz, double sample_rate) {
    current_phase = 0;
    phase_increment = calculate_phase_increment(hz, sample_rate);
}

SquareOscillator::SquareOscillator(const SquareOscillator& other) {
    current_phase = other.current_phase;
    phase_increment = other.phase_increment;
}

double SquareOscillator::get_sample() {
    current_phase = fmod(current_phase + phase_increment, TWO_PI);
    return sign(std::sin(current_phase));
}

void SquareOscillator::set_frequency(double hz, double sample_rate) {
    phase_increment = calculate_phase_increment(hz, sample_rate);
}