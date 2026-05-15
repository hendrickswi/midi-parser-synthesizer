#ifndef MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H

#include "AlgorithmicOscillator.h"

class SineOscillator : public AlgorithmicOscillator {
public:
    SineOscillator();
    SineOscillator(float hz, float sample_rate);
    SineOscillator(const SineOscillator& other);

    float get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H