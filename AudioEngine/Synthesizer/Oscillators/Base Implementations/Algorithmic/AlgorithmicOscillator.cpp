#include "AlgorithmicOscillator.h"

void AlgorithmicOscillator::init(float hz, float sample_rate) {
    base_hz = hz;
    this->sample_rate = sample_rate;
    phase_increment = calculate_phase_increment(hz, sample_rate);
}

AlgorithmicOscillator::AlgorithmicOscillator() {
    init();
}

AlgorithmicOscillator::AlgorithmicOscillator(float hz, float sample_rate) {
    init(hz, sample_rate);
}

void AlgorithmicOscillator::set_modulation_depth(float depth) {
    // Ignored by all AlgorithmicOscillator-extending classes
}

void AlgorithmicOscillator::set_frequency(float hz, float sample_rate) {
    init(hz, sample_rate);
}