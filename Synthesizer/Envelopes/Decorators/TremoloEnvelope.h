#ifndef MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H
#include "EnvelopeDecorator.h"

class TremoloEnvelope : public EnvelopeDecorator {
private:
    float phase;
    float speed;

public:
    TremoloEnvelope(Envelope* env);

    double get_multiplier() override;
};


#endif //MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H