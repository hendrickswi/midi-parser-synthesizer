#include "VibratoOscillator.h"

#include <cmath>

VibratoOscillator::VibratoOscillator(std::unique_ptr<Oscillator> osc, double sample_rate, double base_hz, double speed_hz, double depth)
        : OscillatorDecorator(std::move(osc), sample_rate) {
    this->base_hz = base_hz;
    this->speed_hz = speed_hz;
    this->depth = depth;
    current_phase = 0;
    phase_increment = (speed_hz * TWO_PI) / sample_rate;
}

VibratoOscillator::~VibratoOscillator() = default;

void VibratoOscillator::set_frequency(double hz, double sample_rate) {
    base_hz = hz;
    OscillatorDecorator::set_frequency(hz, sample_rate);
}

double VibratoOscillator::get_sample() {
    double lfo_wobble = std::sin(current_phase);
    current_phase = fmod(current_phase + phase_increment, TWO_PI);

    double pitch_offset = lfo_wobble * depth;
    base_oscillator->set_frequency(base_hz + pitch_offset, sample_rate);
    return base_oscillator->get_sample();

}