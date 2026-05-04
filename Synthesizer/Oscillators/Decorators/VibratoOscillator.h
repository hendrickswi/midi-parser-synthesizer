#ifndef MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H
#include "OscillatorDecorator.h"

class VibratoOscillator : public OscillatorDecorator {
private:
    double current_phase;
    double phase_increment;

    float base_hz;

    float speed_hz;
    float depth;

public:
    VibratoOscillator(std::unique_ptr<Oscillator> osc, double sample_rate = 44100.0f);

};

#endif //MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H