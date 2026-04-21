#ifndef MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H
#include "Oscillator.h"

class SineOscillator : public Oscillator {
private:
    double current_phase;
    double phase_increment;

public:
    SineOscillator();
    SineOscillator(double hz, double sample_rate);
    SineOscillator(const SineOscillator& other);

    double get_sample() override;
    void set_frequency(double hz, double sample_rate) override;
};


#endif //MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H