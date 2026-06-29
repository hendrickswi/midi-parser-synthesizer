#ifndef MIDI_PARSERSYNTHESIZER_LTI_BANDPASSFILTEROSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_LTI_BANDPASSFILTEROSCILLATOR_H

#include "../Base/OscillatorDecorator.h"
#include "../../Filters/LinearTrapezoidalIntegratedSVF.h"

class LTI_BandpassFilterOscillator : public OscillatorDecorator {
private:
    LinearTrapezoidalIntegratedSVF filter;

public:
    LTI_BandpassFilterOscillator();

    void set_params(float sample_rate, float cutoff_hz, float resonance);
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
};

#endif //MIDI_PARSERSYNTHESIZER_LTI_BANDPASSFILTEROSCILLATOR_H