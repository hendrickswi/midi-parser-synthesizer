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
    float attack_max_level;
    float decay_time;
    float sustain_level;
    float release_time;
    float release_min_level;

    float attack_increment;
    float decay_increment;
    float sustain_increment;
    float release_increment;

    void calculate_increments();
    void init(float sample_rate = 44100.0f, float attack_time = 0.1f, float attack_max_level = 1.0f,
        float decay_time = 0.1f, float sustain_level = 0.5f, float release_time = 0.1f, float release_min_level = 0.0f);

public:
    ADSREnvelope();
    ADSREnvelope(float sample_rate, float attack_time, float attack_max_level, float decay_time,
        float sustain_level, float release_time, float release_min_level);
    ADSREnvelope(const ADSREnvelope& other);

    void on() override;
    void off() override;
    void apply_to_block(float* buffer, unsigned int num_frames) override;
    [[nodiscard]] bool is_idle() const override;
    [[nodiscard]] bool is_released() const override;
};


#endif //MIDI_PARSERSYNTHESIZER_ADSRENVELOPE_H