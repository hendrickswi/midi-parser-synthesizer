#ifndef MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H
#include "EnvelopeDecorator.h"

class TremoloEnvelope : public EnvelopeDecorator {
private:
    float TWO_PI = 6.283185f;
    float current_phase;
    float phase_increment;

    float speed_hz;
    float depth;

public:
    TremoloEnvelope();
    ~TremoloEnvelope();

    void apply_to_block(float* buffer, unsigned int num_frames) override;
    void set_params(float sample_rate, float speed_hz, float depth);
};


#endif //MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H