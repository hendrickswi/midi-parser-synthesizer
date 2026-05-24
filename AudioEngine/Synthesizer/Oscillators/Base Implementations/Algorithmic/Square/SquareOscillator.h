#ifndef MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H
#include "../AlgorithmicOscillator.h"

class SquareOscillator : public AlgorithmicOscillator {

private:
    float poly_blep(float t, float dt);

public:
    SquareOscillator();
    SquareOscillator(float hz, float sample_rate);
    SquareOscillator(const SquareOscillator& other);

    float get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H