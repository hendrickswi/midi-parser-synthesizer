#ifndef MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H
#define MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H
#include <cstdint>
#include <functional>

#include "Sample Loading/Sample.h"

class Voice;

class InstrumentRegistry {
private:
    float sample_rate;

    std::array<std::vector<Sample>, 128> melodic_samples;
    std::array<std::vector<Sample>, 128> drum_samples;
    std::array<std::function<void(Voice*, uint8_t, uint8_t)>, 128> melodic_patch_factories;
    std::array<std::function<void(Voice*, uint8_t, uint8_t)>, 128> drum_patch_factories;

    void init(float sample_rate = 44100.0f);
    void init_samples();
    void init_leads();

public:
    InstrumentRegistry();
    InstrumentRegistry(float sample_rate);
    InstrumentRegistry(const InstrumentRegistry& other) = delete;

    void configure_melodic_voice(std::uint8_t patch_id, Voice* voice, uint8_t pitch, uint8_t velocity);
    void configure_drum_voice(std::uint8_t patch_id, Voice* voice, uint8_t pitch, uint8_t velocity);
};


#endif //MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H