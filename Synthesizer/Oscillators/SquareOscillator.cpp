#include "SquareOscillator.h"
#include "Oscillator.h"
#include <cmath>

inline int sign(const double val) {
    return (0.0 < val) - (val < 0.0);
}

SquareOscillator::SquareOscillator() = default;

SquareOscillator::SquareOscillator(double hz, double sample_rate)
    : Oscillator(hz, sample_rate) {
}

SquareOscillator::SquareOscillator(const SquareOscillator& other) = default;

double SquareOscillator::get_sample() {
    current_phase = fmod(current_phase + phase_increment, TWO_PI);
    return sign(std::sin(current_phase));
}