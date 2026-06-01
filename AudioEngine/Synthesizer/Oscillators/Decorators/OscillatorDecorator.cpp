#include "OscillatorDecorator.h"

OscillatorDecorator::OscillatorDecorator(std::unique_ptr<Oscillator> osc, float sample_rate) {
    this->base_oscillator = std::move(osc);
}

OscillatorDecorator::~OscillatorDecorator() = default;

void OscillatorDecorator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    base_oscillator->process_sample_block(buffer, num_frames, fm_buffer);
}

void OscillatorDecorator::set_frequency(float hz, float sample_rate) {
    base_oscillator->set_frequency(hz, sample_rate);
}