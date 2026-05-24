#ifndef MIDI_PARSERSYNTHESIZER_NOISEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_NOISEOSCILLATOR_H
#include "../Base Oscillator/Oscillator.h"


class NoiseOscillator : public Oscillator {
public:
    NoiseOscillator();
    NoiseOscillator(const NoiseOscillator& other);

    float get_sample() override;
    void set_modulation_depth(float depth) override;
    void set_frequency(float hz, float sample_rate) override;
};


#endif //MIDI_PARSERSYNTHESIZER_NOISEOSCILLATOR_H