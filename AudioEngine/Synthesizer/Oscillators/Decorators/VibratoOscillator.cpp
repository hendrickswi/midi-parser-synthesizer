#include "VibratoOscillator.h"

#include <array>
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

void VibratoOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    auto calculated_lfo_buffer = std::array<float, 2048>();
    for (unsigned int i = 0; i < num_frames; i++) {
        float lfo_wobble = std::sin(current_phase);
        float pitch_offset = lfo_wobble * depth;

        if (fm_buffer != nullptr) {
            pitch_offset += fm_buffer[i];
        }
        calculated_lfo_buffer[i] = pitch_offset;
    }

    base_oscillator->process_sample_block(buffer, num_frames, calculated_lfo_buffer.data());
}