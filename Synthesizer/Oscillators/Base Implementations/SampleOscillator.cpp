#include "SampleOscillator.h"

void SampleOscillator::init(const std::vector<float>* sample, float base_frequency) {
    if (sample == nullptr) {
        // TODO: Add default sample data here
    }
    else {
        this->sample = sample;
    }

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

void SampleOscillator::set_frequency(float hz, float sample_rate) {
    playback_speed = hz / base_frequency;
}

float SampleOscillator::get_sample() {
    // TODO: Implement this
    return 0.0f;
}