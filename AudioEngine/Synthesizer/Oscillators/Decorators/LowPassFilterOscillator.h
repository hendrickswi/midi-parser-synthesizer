#ifndef MIDI_PARSERSYNTHESIZER_LOWPASSFILTEROSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_LOWPASSFILTEROSCILLATOR_H
#include "OscillatorDecorator.h"
#include "../Filters/ChamberlinSVF.h"

class LowPassFilterOscillator : public OscillatorDecorator {
private:
    ChamberlinSVF filter;

public:
    LowPassFilterOscillator();

    void set_params(float sample_rate, float cutoff_hz, float resonance);
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
};

#endif //MIDI_PARSERSYNTHESIZER_LOWPASSFILTEROSCILLATOR_H