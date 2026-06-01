#ifndef MIDI_PARSERSYNTHESIZER_DRUMENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_DRUMENVELOPE_H
#include "ADREnvelopeState.h"
#include "../../Envelope.h"

class ADREnvelope : public Envelope {
private:
    ADREnvelopeState state;

    float sample_rate;
    float current_multiplier;

    float attack_time;
    float attack_max_level;
    float decay_time;
    float release_time;
    float release_max_level;
    float release_min_level;

    float attack_increment;
    float decay_increment;
    float release_increment;

    void calculate_rates();
    void init(float sample_rate = 44100.0, float attack_time = 0.01f, float attack_max_level = 1.0f,
        float decay_time = 0.1f, float release_time = 0.1f, float release_max_level = 0.4f, float release_min_level = 0.0f);

public:
    ADREnvelope();
    ADREnvelope(float sample_rate, float attack_time, float attack_max_level, float decay_time,
        float release_time, float release_max_level, float release_min_level);
    ADREnvelope(const ADREnvelope& other);

    void on() override;
    void off() override;
    void apply_to_block(float* buffer, unsigned int num_frames) override;
    [[nodiscard]] bool is_idle() const override;
    [[nodiscard]] bool is_released() const override;
};


#endif //MIDI_PARSERSYNTHESIZER_DRUMENVELOPE_H