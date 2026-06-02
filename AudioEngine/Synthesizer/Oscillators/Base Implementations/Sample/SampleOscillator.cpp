#include "SampleOscillator.h"

#include <cmath>

void SampleOscillator::init(const std::vector<float>* sample, float raw_sample_rate, float target_sample_rate, float base_frequency, float* repeat_low, float* repeat_high) {
    this->sample = sample;
    this->raw_sample_rate = raw_sample_rate;
    this->target_sample_rate = target_sample_rate;
    this->base_frequency = base_frequency;

    if (repeat_low != nullptr && repeat_high != nullptr && sample != nullptr) {
        repeat_enabled = true;
        this->repeat_low_idx = *repeat_low * sample->size();
        this->repeat_high_idx = *repeat_high * sample->size();
    }
    else {
        repeat_enabled = false;
        this->repeat_low_idx = -1;
        this->repeat_high_idx = -1;
    }

    sample_index = 0;
    playback_speed = raw_sample_rate / target_sample_rate;
}

SampleOscillator::SampleOscillator() {
    init(nullptr, 0.0f, 44100.0f, 440.0f, nullptr, nullptr);
}

SampleOscillator::SampleOscillator(const std::vector<float>* sample, float raw_sample_rate, float target_sample_rate, float base_frequency, float* repeat_low, float* repeat_high) {
    init(sample, raw_sample_rate, target_sample_rate, base_frequency, repeat_low, repeat_high);
}

void SampleOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    if (!sample || sample->empty()) {
        std::fill(buffer, buffer + num_frames, 0.0f);
        return;
    }

    for (unsigned int i = 0; i < num_frames; i++) {
        if (static_cast<size_t>(sample_index) >= sample->size()) {
            buffer[i] = 0.0f;
        }
        else {
            int idx_1 = static_cast<int>(sample_index);
            int idx_2 = idx_1 + 1;
            if (idx_2 >= sample->size()) {
                idx_2 = idx_1;
            }

            // Linear interpolation
            float frac = sample_index - idx_1;
            float sample_1 = sample->at(idx_1);
            float sample_2 = sample->at(idx_2);
            buffer[i] = sample_1 + frac * (sample_2 - sample_1);

            // Account for different playback speed due to frequency modulation
            float new_playback_speed = playback_speed;
            if (fm_buffer != nullptr) {
                float current_hz = base_frequency + fm_buffer[i];
                new_playback_speed = current_hz / base_frequency * raw_sample_rate / target_sample_rate;
            }

            // Repeat logic
            sample_index += new_playback_speed;
            if (repeat_enabled && sample_index >= repeat_high_idx) {
                sample_index = repeat_low_idx;
            }
        }
    }
}

void SampleOscillator::set_modulation_depth(float depth) {
}

void SampleOscillator::set_frequency(float hz, float sample_rate) {
    target_sample_rate = sample_rate;
    playback_speed = hz / base_frequency * raw_sample_rate / sample_rate;
}