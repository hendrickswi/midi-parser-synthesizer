#ifndef MIDI_PARSERSYNTHESIZER_LINEARTRAPEZOIDALINTEGRATEDSVF_H
#define MIDI_PARSERSYNTHESIZER_LINEARTRAPEZOIDALINTEGRATEDSVF_H

class LinearTrapezoidalIntegratedSVF {
private:
    float PI = 3.141592;
    float ic1eq;
    float ic2eq;
    float a1, a2, a3, k;

public:
    LinearTrapezoidalIntegratedSVF();

    void set_params(float sample_rate, float cutoff_hz, float resonance);
    float low_pass(float sample);
    float band_pass(float sample);
    float high_pass(float sample);
};

#endif //MIDI_PARSERSYNTHESIZER_LINEARTRAPEZOIDALINTEGRATEDSVF_H