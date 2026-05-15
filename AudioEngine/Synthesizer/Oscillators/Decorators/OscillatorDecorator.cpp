#include "OscillatorDecorator.h"

OscillatorDecorator::OscillatorDecorator(std::unique_ptr<Oscillator> osc, float sample_rate) {
    this->base_oscillator = std::move(osc);
}

OscillatorDecorator::~OscillatorDecorator() = default;

void OscillatorDecorator::set_frequency(float hz, float sample_rate) {
    base_oscillator->set_frequency(hz, sample_rate);
}

float OscillatorDecorator::get_sample() {
    return base_oscillator->get_sample();
}