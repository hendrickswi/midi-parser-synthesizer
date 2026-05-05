#ifndef MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H
#include "Oscillator.h"


class SawtoothOscillator : public Oscillator {
public:
    SawtoothOscillator();
    SawtoothOscillator(float hz, float sample_rate);
    SawtoothOscillator(const SawtoothOscillator& other);

    float get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H