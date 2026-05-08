#ifndef MIDI_PARSERSYNTHESIZER_TRIANGLEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_TRIANGLEOSCILLATOR_H
#include "Oscillator.h"


class TriangleOscillator : public Oscillator {
public:
    TriangleOscillator();
    TriangleOscillator(float hz, float sample_rate);
    TriangleOscillator(const TriangleOscillator& other);

    float get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_TRIANGLEOSCILLATOR_H