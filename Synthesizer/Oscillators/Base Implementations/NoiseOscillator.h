#ifndef MIDI_PARSERSYNTHESIZER_NOISEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_NOISEOSCILLATOR_H
#include "Oscillator.h"


class NoiseOscillator : public Oscillator {
public:
    NoiseOscillator();
    NoiseOscillator(const NoiseOscillator& other);

    float get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_NOISEOSCILLATOR_H