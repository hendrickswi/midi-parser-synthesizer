#ifndef MIDI_PARSERSYNTHESIZER_HIGHPASSFILTEROSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_HIGHPASSFILTEROSCILLATOR_H
#include "../Base/OscillatorDecorator.h"
#include "../../Filters/ChamberlinSVF.h"

class ChamberlinHighpassFilterOscillator : public OscillatorDecorator {
private:
    ChamberlinSVF filter;

public:
    ChamberlinHighpassFilterOscillator();

    void set_params(float sample_rate, float cutoff_hz, float resonance);
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
};

#endif //MIDI_PARSERSYNTHESIZER_HIGHPASSFILTEROSCILLATOR_H