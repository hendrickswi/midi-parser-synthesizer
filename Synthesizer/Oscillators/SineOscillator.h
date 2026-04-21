#ifndef MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H
#include "Oscillator.h"


class SineOscillator : public Oscillator {
private:
    double current_phase;
    double phase_increment;

public:
    SineOscillator();
    SineOscillator(float hz, float sample_rate);
    SineOscillator(const SineOscillator& other);

    void set_frequency(float hz, float sample_rate) override;
    double get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H