#include "SineOscillator.h"
#include "../../../../Lookup Tables/SineLookupTable.h"

SineOscillator::SineOscillator() = default;

SineOscillator::SineOscillator(float hz, float sample_rate)
    : AlgorithmicOscillator(hz, sample_rate) {
}

SineOscillator::SineOscillator(const SineOscillator& other) = default;

void SineOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    float table_size = static_cast<float>(SineLookupTable::TABLE_SIZE);

    for (int i = 0; i < num_frames; i++) {
        float normalized_phase = current_phase / TWO_PI;
        buffer[i] = SineLookupTable::sin(table_size * normalized_phase);

        float new_phase_increment = phase_increment;
        if (fm_buffer != nullptr) {
            float current_hz = base_hz + fm_buffer[i];
            new_phase_increment = calculate_phase_increment(current_hz, sample_rate);
        }

        current_phase += new_phase_increment;
        if (current_phase >= TWO_PI) {
            current_phase -= TWO_PI;
        }
    }
}