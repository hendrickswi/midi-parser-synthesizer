#ifndef MIDI_PARSERSYNTHESIZER_ENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_ENVELOPE_H

class Envelope {
public:
    virtual ~Envelope() = default;

    virtual void on() = 0;
    virtual void off() = 0;
    [[nodiscard]] virtual double get_multiplier() = 0;
    [[nodiscard]] virtual bool is_idle() const = 0;
    [[nodiscard]] virtual bool is_released() const = 0;
};


#endif //MIDI_PARSERSYNTHESIZER_ENVELOPE_H