#include "ChamberlinSVF.h"

#include <algorithm>
#include <cmath>

ChamberlinSVF::ChamberlinSVF()
    : low(0.0f), band(0.0f), freq_scaling_factor(0.0f), damping_factor(0.0f) {
}

void ChamberlinSVF::set_params(float sample_rate, float cutoff_hz, float resonance) {
    float max_cutoff = sample_rate / 6.0f;
    cutoff_hz = std::clamp(cutoff_hz, 10.0f, max_cutoff);
    freq_scaling_factor = 2.0f * std::sin(PI * cutoff_hz / sample_rate);
    damping_factor = 1.0f - std::clamp(resonance, 0.0001f, 0.9999f);
}

float ChamberlinSVF::low_pass(float sample) {
    float high = sample - (low + damping_factor * band);
    band += freq_scaling_factor * high;
    low += freq_scaling_factor * band;

    return low;
}

float ChamberlinSVF::band_pass(float sample) {
    float high = sample - (low + damping_factor * band);
    band += freq_scaling_factor * high;
    low += freq_scaling_factor * band;

    return band;
}

float ChamberlinSVF::high_pass(float sample) {
    float high = sample - (low + damping_factor * band);
    band += freq_scaling_factor * high;
    low += freq_scaling_factor * band;

    return high;
}