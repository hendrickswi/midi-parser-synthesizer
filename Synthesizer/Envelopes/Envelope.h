#ifndef MIDI_PARSERSYNTHESIZER_ENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_ENVELOPE_H
#include <memory>

class Envelope {
public:
    virtual ~Envelope() = default;

    virtual void on() = 0;
    virtual void off() = 0;
    [[nodiscard]] virtual float get_multiplier() = 0;
    [[nodiscard]] virtual bool is_finished() const = 0;
};


#endif //MIDI_PARSERSYNTHESIZER_ENVELOPE_H