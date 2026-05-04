#ifndef MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H
#include "Oscillator.h"

class SineOscillator : public Oscillator {
public:
    SineOscillator();
    SineOscillator(double hz, double sample_rate);
    SineOscillator(const SineOscillator& other);

    double get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H