#ifndef MIDI_PARSERSYNTHESIZER_CHAMBERLINSVF_H
#define MIDI_PARSERSYNTHESIZER_CHAMBERLINSVF_H

class ChamberlinSVF {
private:
    float PI = 3.141592;
    float low;
    float band;
    float freq_scaling_factor;
    float damping_factor;

public:
    ChamberlinSVF();

    void set_params(float sample_rate, float cutoff_hz, float resonance);
    float low_pass(float sample);
    float band_pass(float sample);
    float high_pass(float sample);
};

#endif //MIDI_PARSERSYNTHESIZER_CHAMBERLINSVF_H