#ifndef MIDI_PARSERSYNTHESIZER_ADSRENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_ADSRENVELOPE_H
#include "../Envelope.h"

class ADSREnvelope : public Envelope {
private:
    EnvelopeState state;
    float sample_rate;
    float current_multiplier;

    float attack_time;
    float decay_time;
    float sustain_level;
    float release_time;

    float attack_increment;
    float decay_increment;
    float sustain_increment;
    float release_increment;

    void recalculate_rates();

public:
    ADSREnvelope();
    ADSREnvelope(float sample_rate);
    ADSREnvelope(const ADSREnvelope& other);

    void on() override;
    void off() override;
    [[nodiscard]] double get_multiplier() override;
    [[nodiscard]] const EnvelopeState& get_state() const override;

    void set_attack(float seconds);
    void set_decay(float seconds);
    void set_sustain(float level);
    void set_release(float seconds);
};


#endif //MIDI_PARSERSYNTHESIZER_ADSRENVELOPE_H