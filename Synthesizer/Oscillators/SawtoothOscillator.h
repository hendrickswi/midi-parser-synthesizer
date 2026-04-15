#ifndef MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H
#include "Oscillator.h"


class SawtoothOscillator : public Oscillator {
private:
    float current_phase;
    float phase_increment;

public:
    SawtoothOscillator();
    SawtoothOscillator(float hz, float sample_rate);
    SawtoothOscillator(const SawtoothOscillator& other);

    void set_frequency(float hz, float sample_rate) override;
    float get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H