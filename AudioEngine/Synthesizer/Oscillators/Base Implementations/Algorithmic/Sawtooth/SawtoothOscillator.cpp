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

void SawtoothOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    for (int i = 0; i < num_frames; i++) {
        float new_phase_increment = phase_increment;
        if (fm_buffer != nullptr) {
            float current_hz = base_hz + fm_buffer[i];
            new_phase_increment = calculate_phase_increment(current_hz, sample_rate);
        }

        float normalized_phase = current_phase / TWO_PI;
        float dt = new_phase_increment / TWO_PI;
        float raw_wave = 2.0f * normalized_phase - 1.0f;
        buffer[i] = raw_wave - poly_blep(normalized_phase, dt);

        current_phase += new_phase_increment;
        if (current_phase >= TWO_PI) {
            current_phase -= TWO_PI;
        }
    }
}