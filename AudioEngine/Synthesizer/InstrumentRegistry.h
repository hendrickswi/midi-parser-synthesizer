#ifndef MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H
#define MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H
#include <cstdint>
#include <functional>
#include <memory>

class Voice;

class InstrumentRegistry {
private:
    float sample_rate;

    // Raw pointers because don't want to take ownership, just need temporary access
    std::array<std::function<void(Voice*)>, 128> patch_factories;
    std::array<std::function<void(Voice*)>, 128> drum_patch_factories;

    void init_pianos();
    void init_organs();
    void init_leads();
    void init_drums();
    void init(float sample_rate = 44100.0f);

public:
    InstrumentRegistry();
    InstrumentRegistry(float sample_rate);
    InstrumentRegistry(const InstrumentRegistry& other) = delete;

    void configure_melodic_voice(std::uint8_t patch_id, Voice* voice);
    void configure_drum_voice(std::uint8_t patch_id, Voice* voice);
};


#endif //MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H