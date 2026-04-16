#ifndef MIDI_PARSERSYNTHESIZER_ENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_ENVELOPE_H
#include <memory>

#include "Base Implementations/EnvelopeState.h"

class Envelope {
public:
    virtual ~Envelope() = default;

    virtual void on() = 0;
    virtual void off() = 0;
    [[nodiscard]] virtual float get_multiplier() = 0;
    [[nodiscard]] virtual const EnvelopeState& get_state() const = 0;
};


#endif //MIDI_PARSERSYNTHESIZER_ENVELOPE_H