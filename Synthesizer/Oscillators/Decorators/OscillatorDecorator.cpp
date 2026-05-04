#include "OscillatorDecorator.h"

OscillatorDecorator::OscillatorDecorator(std::unique_ptr<Oscillator> osc, double sample_rate)
{
    this->base_oscillator = std::move(osc);
    this->sample_rate = sample_rate;
}

OscillatorDecorator::~OscillatorDecorator() = default;

void OscillatorDecorator::set_frequency(double hz, double sample_rate) {
    base_oscillator->set_frequency(hz, sample_rate);
}

double OscillatorDecorator::get_sample() {
    return base_oscillator->get_sample();
}