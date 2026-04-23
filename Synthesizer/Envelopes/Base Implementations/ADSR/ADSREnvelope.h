#ifndef MIDI_PARSERSYNTHESIZER_ADSRENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_ADSRENVELOPE_H
#include "ADSREnvelopeState.h"
#include "../../Envelope.h"

class ADSREnvelope : public Envelope {
private:
    ADSREnvelopeState state;

    double sample_rate;
    double current_multiplier;

    double attack_time;
    double decay_time;
    double sustain_level;
    double release_time;

    double attack_increment;
    double decay_increment;
    double sustain_increment;
    double release_increment;

    void recalculate_rates();
    void init(double sample_rate = 44100.0, double attack_time = 0.1, double decay_time = 0.1, double sustain_level = 0.5, double release_time = 0.1);

public:
    ADSREnvelope();
    ADSREnvelope(double sample_rate, double attack_time, double decay_time, double sustain_level, double release_time);
    ADSREnvelope(const ADSREnvelope& other);

    void on() override;
    void off() override;
    [[nodiscard]] double get_multiplier() override;
    [[nodiscard]] bool is_idle() const override;
    [[nodiscard]] bool is_released() const override;

    void set_attack(double seconds);
    void set_decay(double seconds);
    void set_sustain(double level);
    void set_release(double seconds);
};


#endif //MIDI_PARSERSYNTHESIZER_ADSRENVELOPE_H