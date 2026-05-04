#ifndef MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H
#include "OscillatorDecorator.h"

class VibratoOscillator : public OscillatorDecorator {
private:
    double current_phase;
    double phase_increment;

    double base_hz;
    double speed_hz;
    double depth;

public:
    VibratoOscillator(std::unique_ptr<Oscillator> osc, double sample_rate = 44100.0f, double base_hz = 4.0, double speed_hz = 5.0, double depth = 0.5);
    ~VibratoOscillator();

    void set_frequency(double hz, double sample_rate) override;
    double get_sample() override;

};

#endif //MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H