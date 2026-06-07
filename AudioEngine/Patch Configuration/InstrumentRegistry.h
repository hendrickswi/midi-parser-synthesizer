#ifndef MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H
#define MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H
#include <array>
#include <cstdint>

#include "PatchDefinition.h"

class Voice;

class InstrumentRegistry {
private:
    float sample_rate;
    std::array<PatchDefinition, 128> melodic_patches;
    std::array<uint8_t, 128> melodic_patch_aliases;
    std::array<PatchDefinition, 128> drum_patches;
    std::array<uint8_t, 128> drum_patch_aliases;

    void init_samples();
    void init_envelopes();
    void init_leads();
    void init_pads();
    void set_fallbacks();


public:
    explicit InstrumentRegistry(float sample_rate = 48000.0f);

    [[nodiscard]] const PatchDefinition* get_melodic_patch_config(uint8_t patch_id) const;
    [[nodiscard]] const PatchDefinition* get_drum_patch_config(uint8_t pitch_key) const;
};


#endif //MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H