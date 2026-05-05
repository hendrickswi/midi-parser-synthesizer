#ifndef MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H
#include "Oscillator.h"

class SquareOscillator : public Oscillator {
public:
    SquareOscillator();
    SquareOscillator(float hz, float sample_rate);
    SquareOscillator(const SquareOscillator& other);

    float get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H