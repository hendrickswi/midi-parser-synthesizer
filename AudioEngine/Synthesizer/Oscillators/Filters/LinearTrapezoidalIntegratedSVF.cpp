#include "LinearTrapezoidalIntegratedSVF.h"

#include <algorithm>
#include <cmath>

LinearTrapezoidalIntegratedSVF::LinearTrapezoidalIntegratedSVF()
    : ic1eq(0.0f), ic2eq(0.0f), a1(0.0f), a2(0.0f), a3(0.0f), k(0.0f) {
}

void LinearTrapezoidalIntegratedSVF::set_params(float sample_rate, float cutoff_hz, float resonance) {
    cutoff_hz = std::clamp(cutoff_hz, 10.0f, sample_rate/ 2.0f);
    resonance = std::clamp(resonance, 0.1f, 20.0f);

    float g = std::tan(PI * cutoff_hz / sample_rate);
    k = 1.0f / resonance;

    a1 = 1.0f / (1.0f + g * (g + k));
    a2 = g * a1;
    a3 = g * a2;
}

float LinearTrapezoidalIntegratedSVF::low_pass(float sample) {
    float v3 = sample - ic2eq;
    float v1 = a1 * ic1eq + a2 * v3;
    float v2 = ic2eq + a2 * ic1eq + a3 * v3;

    ic1eq = 2.0f * v1 - ic1eq;
    ic2eq = 2.0f * v2 - ic2eq;

    return v2;
}

float LinearTrapezoidalIntegratedSVF::band_pass(float sample) {
    float v3 = sample - ic2eq;
    float v1 = a1 * ic1eq + a2 * v3;
    float v2 = ic2eq + a2 * ic1eq + a3 * v3;

    ic1eq = 2.0f * v1 - ic1eq;
    ic2eq = 2.0f * v2 - ic2eq;

    return v1;
}

float LinearTrapezoidalIntegratedSVF::high_pass(float sample) {
    float v3 = sample - ic2eq;
    float v1 = a1 * ic1eq + a2 * v3;
    float v2 = ic2eq + a2 * ic1eq + a3 * v3;

    ic1eq = 2.0f * v1 - ic1eq;
    ic2eq = 2.0f * v2 - ic2eq;

    return sample - k * v1 - v2;
}