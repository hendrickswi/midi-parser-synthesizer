#ifndef MIDI_PARSERSYNTHESIZER_BANDPASSFILTEROSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_BANDPASSFILTEROSCILLATOR_H
#include "OscillatorDecorator.h"
#include "../Filters/ChamberlinSVF.h"

class BandpassFilterOscillator : public OscillatorDecorator {
private:
    ChamberlinSVF filter;

public:
    BandpassFilterOscillator();

    void set_params(float sample_rate, float cutoff_hz, float resonance);
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
};


#endif //MIDI_PARSERSYNTHESIZER_BANDPASSFILTEROSCILLATOR_H