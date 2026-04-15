#ifndef MIDI_PARSERSYNTHESIZER_ENVELOPEDECORATOR_H
#define MIDI_PARSERSYNTHESIZER_ENVELOPEDECORATOR_H
#include "../Envelope.h"

class EnvelopeDecorator : public Envelope {
protected:
    std::unique_ptr<Envelope> base_envelope;

public:
    EnvelopeDecorator(std::unique_ptr<Envelope> env) : base_envelope(std::move(env)) {}

    void on() override {
        base_envelope->on();
    }
    void off() override {
        base_envelope->off();
    }

    // Default implementations
    float get_multiplier() override {
        return base_envelope->get_multiplier();
    }
    [[nodiscard]] bool is_finished() const override {
        return base_envelope->is_finished();
    }
};


#endif //MIDI_PARSERSYNTHESIZER_ENVELOPEDECORATOR_H