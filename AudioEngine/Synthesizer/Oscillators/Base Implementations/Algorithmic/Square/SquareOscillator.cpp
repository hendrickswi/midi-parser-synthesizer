#include "SquareOscillator.h"
#include <cmath>

SquareOscillator::SquareOscillator() = default;

SquareOscillator::SquareOscillator(float hz, float sample_rate)
    : AlgorithmicOscillator(hz, sample_rate) {
}

SquareOscillator::SquareOscillator(const SquareOscillator& other) = default;

float SquareOscillator::poly_blep(float t, float dt) {
    if (t < dt) {
        t /= dt;
        return t + t - t * t - 1.0f;
    }
    else if (t > 1.0f - dt) {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

void SquareOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    for (int i = 0; i < num_frames; i++) {
        float new_phase_increment = phase_increment;
        if (fm_buffer != nullptr) {
            float current_hz = base_hz + fm_buffer[i];
            new_phase_increment = calculate_phase_increment(current_hz, sample_rate);
        }

        float normalized_phase = current_phase / TWO_PI;
        float dt = new_phase_increment / TWO_PI;
        float raw_wave = (normalized_phase < 0.5f) ? 1.0f : -1.0f;

        // Upward transition at normalized phase 0.0
        float correction_up = poly_blep(normalized_phase, dt);

        float t_2 = normalized_phase + 0.5f;
        if (t_2 >= 1.0f) {
            t_2 -= 1.0f;
        }
        // Downward transition at normalized phase 0.5
        float correction_down = poly_blep(t_2, dt);

        // 1/sqrt(2) attenuation to align RMS power with other algorithmic oscillators
        // Also prevents PolyBLEP overshoot causing clicks during playback
        buffer[i] = raw_wave + correction_up - correction_down * 0.707f;

        // Increment logic
        current_phase += new_phase_increment;
        if (current_phase >= TWO_PI) {
            current_phase -= TWO_PI;
        }
    }
}