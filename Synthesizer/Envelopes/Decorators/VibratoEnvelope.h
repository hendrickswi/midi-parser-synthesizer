#ifndef MIDI_PARSERSYNTHESIZER_VIBRATOENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_VIBRATOENVELOPE_H
#include "EnvelopeDecorator.h"


class VibratoEnvelope : public EnvelopeDecorator {
private:
    double current_phase;
    double phase_increment;

public:
    VibratoEnvelope(std::unique_ptr<Envelope> env);
    ~VibratoEnvelope();

    double get_multiplier() override;
};


#endif //MIDI_PARSERSYNTHESIZER_VIBRATOENVELOPE_H