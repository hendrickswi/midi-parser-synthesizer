#ifndef MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H
#include "Oscillator.h"


class SawtoothOscillator : public Oscillator {
public:
    SawtoothOscillator();
    SawtoothOscillator(double hz, double sample_rate);
    SawtoothOscillator(const SawtoothOscillator& other);

    double get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H