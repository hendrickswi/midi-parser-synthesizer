#ifndef MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H
#define MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H

#include "../../Base Implementations/Base Oscillator/Oscillator.h"

class OscillatorDecorator : public Oscillator {
protected:
    Oscillator* base_oscillator;

    OscillatorDecorator();
    ~OscillatorDecorator();

public:
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
    void set_frequency(float hz, float sample_rate) override;
    void set_modulation_depth(float depth) override;
    void set_base_oscillator(Oscillator* base_oscillator);
};


#endif //MIDI_PARSERSYNTHESIZER_OSCILLATORDECORATOR_H