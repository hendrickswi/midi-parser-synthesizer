#ifndef MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H
#define MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H
#include <memory>

#include "../Base Implementations/Base Oscillator/Oscillator.h"



class OscillatorDecorator : public Oscillator {
protected:
    std::unique_ptr<Oscillator> base_oscillator;

    OscillatorDecorator(std::unique_ptr<Oscillator> osc, float sample_rate = 44100.0f);
    ~OscillatorDecorator();

public:
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
    void set_frequency(float hz, float sample_rate) override;
};


#endif //MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H