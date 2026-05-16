#include "SampleOscillator.h"

#include <cmath>

void SampleOscillator::init(const std::vector<float>* sample, float base_frequency) {
    this->sample = sample;
    sample_index = 0;
    playback_speed = 1.0f;
    this->base_frequency = base_frequency;
}

SampleOscillator::SampleOscillator() {
    init();
}

SampleOscillator::SampleOscillator(const std::vector<float>* sample, float base_frequency) {
    init(sample, base_frequency);
}



float SampleOscillator::get_sample() {
    if (!sample || sample->empty() || sample_index >= sample->size()) return 0.0f;

    int idx_1 = std::floor(sample_index);
    int idx_2 = idx_1 + 1;
    if (idx_2 >= sample->size()) {
        idx_2 = idx_1;
    }

    // Linear interpolation
    float frac = sample_index - idx_1;
    float sample_1 = sample->at(idx_1);
    float sample_2 = sample->at(idx_2);
    float interpolated_sample = sample_1 + frac * (sample_2 - sample_1);

    sample_index += playback_speed;

    return interpolated_sample;
}

void SampleOscillator::set_modulation_depth(float depth) {
}

void SampleOscillator::set_frequency(float hz, float sample_rate) {
    playback_speed = hz / base_frequency;
}