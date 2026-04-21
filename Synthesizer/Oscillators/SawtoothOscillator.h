#ifndef MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H
#include "Oscillator.h"


class SawtoothOscillator : public Oscillator {
private:
    double current_phase;
    double phase_increment;

public:
    SawtoothOscillator();
    SawtoothOscillator(double hz, double sample_rate);
    SawtoothOscillator(const SawtoothOscillator& other);

    double get_sample() override;
    void set_frequency(double hz, double sample_rate) override;
};


#endif //MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H