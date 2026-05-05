#ifndef MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H
#include "OscillatorDecorator.h"

class VibratoOscillator : public OscillatorDecorator {
private:
    float current_phase;
    float phase_increment;

    float base_hz;
    float speed_hz;
    float depth;

public:
    VibratoOscillator(std::unique_ptr<Oscillator> osc, float sample_rate = 44100.0f, float base_hz = 4.0, float speed_hz = 5.0, float depth = 0.5);
    ~VibratoOscillator();

    void set_frequency(float hz, float sample_rate) override;
    float get_sample() override;

};

#endif //MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H