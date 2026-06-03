#ifndef MIDI_PARSERSYNTHESIZER_ENVELOPEDECORATOR_H
#define MIDI_PARSERSYNTHESIZER_ENVELOPEDECORATOR_H
#include "../Envelope.h"

class EnvelopeDecorator : public Envelope {
protected:
    Envelope* base_envelope;
    float sample_rate;

    EnvelopeDecorator();
    ~EnvelopeDecorator();

public:
    void on() override;
    void off() override;
    void apply_to_block(float* buffer, unsigned int num_frames) override;
    [[nodiscard]] bool is_idle() const override;
    [[nodiscard]] bool is_released() const override;
    void set_base_envelope(Envelope* envelope);
};


#endif //MIDI_PARSERSYNTHESIZER_ENVELOPEDECORATOR_H