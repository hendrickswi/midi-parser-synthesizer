#include "OscillatorDecorator.h"

OscillatorDecorator::OscillatorDecorator() {
    base_oscillator = nullptr;
    sample_rate = 48000.0f;
}

OscillatorDecorator::~OscillatorDecorator() = default;

void OscillatorDecorator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    if (base_oscillator == nullptr) return;
    base_oscillator->process_sample_block(buffer, num_frames, fm_buffer);
}

void OscillatorDecorator::set_frequency(float hz, float sample_rate) {
    if (base_oscillator == nullptr) return;
    base_oscillator->set_frequency(hz, sample_rate);
}

void OscillatorDecorator::set_modulation_depth(float depth) {
    if (base_oscillator == nullptr) return;
    base_oscillator->set_modulation_depth(depth);
}

void OscillatorDecorator::set_base_oscillator(Oscillator* base_oscillator) {
    this->base_oscillator = base_oscillator;
}