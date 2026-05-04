#ifndef MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H
#define MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H
#include <memory>

#include "../Base Implementations/Oscillator.h"


class OscillatorDecorator : public Oscillator {
private:
    std::unique_ptr<Oscillator> base_oscillator;
    double sample_rate;

public:
    OscillatorDecorator(std::unique_ptr<Oscillator> osc, double sample_rate = 44100.0f);
    ~OscillatorDecorator();

    void set_frequency(double hz, double sample_rate) override;
    double get_sample() override;
};


#endif //MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H