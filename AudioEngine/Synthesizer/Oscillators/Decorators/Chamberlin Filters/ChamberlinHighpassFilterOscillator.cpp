#include "ChamberlinHighpassFilterOscillator.h"

ChamberlinHighpassFilterOscillator::ChamberlinHighpassFilterOscillator() {
    filter = ChamberlinSVF();
}

void ChamberlinHighpassFilterOscillator::set_params(float sample_rate, float cutoff_hz, float resonance) {
    filter.set_params(sample_rate, cutoff_hz, resonance);
}

void ChamberlinHighpassFilterOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    base_oscillator->process_sample_block(buffer, num_frames, fm_buffer);
    for (int i = 0; i < num_frames; i++) {
        buffer[i] = filter.high_pass(buffer[i]);
    }
}