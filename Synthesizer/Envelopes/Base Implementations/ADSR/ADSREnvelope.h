#ifndef MIDI_PARSERSYNTHESIZER_ADSRENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_ADSRENVELOPE_H
#include "ADSREnvelopeState.h"
#include "../../Envelope.h"

class ADSREnvelope : public Envelope {
private:
    ADSREnvelopeState state;

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
    void init(float sample_rate = 44100.0, float attack_time = 0.1, float decay_time = 0.1, float sustain_level = 0.5, float release_time = 0.1);

public:
    ADSREnvelope();
    ADSREnvelope(float sample_rate, float attack_time, float decay_time, float sustain_level, float release_time);
    ADSREnvelope(const ADSREnvelope& other);

    void on() override;
    void off() override;
    [[nodiscard]] float get_multiplier() override;
    [[nodiscard]] bool is_idle() const override;
    [[nodiscard]] bool is_released() const override;
};


#endif //MIDI_PARSERSYNTHESIZER_ADSRENVELOPE_H