#ifndef MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H
#include "Oscillator.h"

class SquareOscillator : public Oscillator {
public:
    SquareOscillator();
    SquareOscillator(double hz, double sample_rate);
    SquareOscillator(const SquareOscillator& other);

    double get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H