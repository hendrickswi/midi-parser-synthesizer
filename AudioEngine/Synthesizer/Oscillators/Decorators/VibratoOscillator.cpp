#include "VibratoOscillator.h"

#include <cmath>

VibratoOscillator::VibratoOscillator(std::unique_ptr<Oscillator> osc, float sample_rate, float base_hz, float speed_hz, float depth)
        : OscillatorDecorator(std::move(osc), sample_rate) {
    this->base_hz = base_hz;
    this->speed_hz = speed_hz;
    this->depth = depth;
    current_phase = 0;
    phase_increment = (speed_hz * TWO_PI) / sample_rate;
}

VibratoOscillator::~VibratoOscillator() = default;

void VibratoOscillator::set_frequency(float hz, float sample_rate) {
    base_hz = hz;
    OscillatorDecorator::set_frequency(hz, sample_rate);
}

void VibratoOscillator::set_modulation_depth(float depth) {
    this->depth = depth;
}

float VibratoOscillator::get_sample() {
    float lfo_wobble = std::sin(current_phase);
    current_phase = fmod(current_phase + phase_increment, TWO_PI);

    float pitch_offset = lfo_wobble * depth;
    base_oscillator->set_frequency(base_hz + pitch_offset, sample_rate);
    return base_oscillator->get_sample();

}