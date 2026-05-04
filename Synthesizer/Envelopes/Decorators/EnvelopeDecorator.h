#ifndef MIDI_PARSERSYNTHESIZER_ENVELOPEDECORATOR_H
#define MIDI_PARSERSYNTHESIZER_ENVELOPEDECORATOR_H
#include "../Envelope.h"
#include <memory>

class EnvelopeDecorator : public Envelope {
protected:
    std::unique_ptr<Envelope> base_envelope;
    double sample_rate;

    EnvelopeDecorator(std::unique_ptr<Envelope> env, double sample_rate = 44100.0f);
    ~EnvelopeDecorator();

public:
    void on() override;
    void off() override;
    double get_multiplier() override;
    [[nodiscard]] bool is_idle() const override;
    [[nodiscard]] bool is_released() const override;
};


#endif //MIDI_PARSERSYNTHESIZER_ENVELOPEDECORATOR_H