#include "VibratoOscillator.h"

#include "../../../Lookup Tables/SineLookupTable.h"
#include <vector>

VibratoOscillator::VibratoOscillator() {
    this->base_hz = 4.0f;
    this->speed_hz = 5.0f;
    this->depth = 0.5f;
    sample_rate = 48000.0f;
    current_phase = 0;
    phase_increment = speed_hz * TWO_PI / sample_rate;
    calculated_lfo_buffer = std::vector<float>(4096); // Safe estimate to prevent resizing
}

VibratoOscillator::~VibratoOscillator() = default;

void VibratoOscillator::set_frequency(float hz, float sample_rate) {
    base_hz = hz;
    base_oscillator->set_frequency(base_hz, sample_rate);
    current_phase = 0;
}

void VibratoOscillator::set_modulation_depth(float depth) {
    this->depth = depth;
}

void VibratoOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    float table_size = static_cast<float>(SineLookupTable::TABLE_SIZE);

    for (unsigned int i = 0; i < num_frames; i++) {
        float normalized_phase = current_phase / TWO_PI;
        float lfo_wobble = SineLookupTable::sin(table_size * normalized_phase);
        float pitch_offset = lfo_wobble * depth;

        if (fm_buffer != nullptr) {
            pitch_offset += fm_buffer[i];
        }
        calculated_lfo_buffer[i] = pitch_offset;
    }

    base_oscillator->process_sample_block(buffer, num_frames, calculated_lfo_buffer.data());
}

void VibratoOscillator::set_params(float sample_rate, float base_hz, float speed_hz, float depth) {
    this->sample_rate = sample_rate;
    this->base_hz = base_hz;
    this->speed_hz = speed_hz;
    this->depth = depth;
    phase_increment = speed_hz * TWO_PI / sample_rate;
}