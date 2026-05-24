#include "SawtoothOscillator.h"

SawtoothOscillator::SawtoothOscillator() = default;

SawtoothOscillator::SawtoothOscillator(float hz, float sample_rate)
    : AlgorithmicOscillator(hz, sample_rate) {
}

SawtoothOscillator::SawtoothOscillator(const SawtoothOscillator& other) = default;

float SawtoothOscillator::poly_blep(float normalized_phase, float dt) {
    if (normalized_phase < dt) {
        normalized_phase /= dt;
        return normalized_phase + normalized_phase - normalized_phase * normalized_phase - 1.0f;
    }
    else if (normalized_phase > 1.0f - dt) {
        normalized_phase = (normalized_phase - 1.0f) / dt;
        return normalized_phase + normalized_phase - normalized_phase * normalized_phase + 1.0f;
    }
    return 0.0f;
}

float SawtoothOscillator::get_sample() {
    current_phase += phase_increment;
    if (current_phase >= TWO_PI) {
        current_phase -= TWO_PI;
    }

    float normalized_phase = current_phase / TWO_PI;
    float dt = phase_increment / TWO_PI;
    float raw_wave = 2.0f * normalized_phase - 1.0f;
    return raw_wave - poly_blep(normalized_phase, dt);
}