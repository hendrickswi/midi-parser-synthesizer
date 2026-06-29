//
// Created by williamh on 6/28/26.
//

#ifndef MIDI_PARSERSYNTHESIZER_LTI_HIGHPASSFILTEROSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_LTI_HIGHPASSFILTEROSCILLATOR_H

#include "../Base/OscillatorDecorator.h"
#include "../../Filters/LinearTrapezoidalIntegratedSVF.h"

class LTI_HighpassFilterOscillator : public OscillatorDecorator {
private:
    LinearTrapezoidalIntegratedSVF filter;

public:
    LTI_HighpassFilterOscillator();

    void set_params(float sample_rate, float cutoff_hz, float resonance);
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
};


#endif //MIDI_PARSERSYNTHESIZER_LTI_HIGHPASSFILTEROSCILLATOR_H