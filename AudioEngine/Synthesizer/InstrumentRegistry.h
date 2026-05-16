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
    std::array<std::function<void(Voice*, uint8_t)>, 128> melodic_patch_factories;
    std::array<std::function<void(Voice*, uint8_t)>, 128> drum_patch_factories;

    void init(float sample_rate = 44100.0f);
    void init_samples();

public:
    InstrumentRegistry();
    InstrumentRegistry(float sample_rate);
    InstrumentRegistry(const InstrumentRegistry& other) = delete;

    void configure_melodic_voice(std::uint8_t patch_id, Voice* voice, uint8_t pitch);
    void configure_drum_voice(std::uint8_t patch_id, Voice* voice, uint8_t pitch);
};


#endif //MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H