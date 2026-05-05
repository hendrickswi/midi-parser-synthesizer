#ifndef MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H
#define MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H
#include <memory>

#include "../Base Implementations/Oscillator.h"


class OscillatorDecorator : public Oscillator {
protected:
    std::unique_ptr<Oscillator> base_oscillator;
    float sample_rate;

    OscillatorDecorator(std::unique_ptr<Oscillator> osc, float sample_rate = 44100.0f);
    ~OscillatorDecorator();

public:
    void set_frequency(float hz, float sample_rate) override;
    float get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H