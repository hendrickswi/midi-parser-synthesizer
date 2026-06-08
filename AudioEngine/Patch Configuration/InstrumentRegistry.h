#ifndef MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H
#define MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H
#include <array>
#include <cstdint>
#include <nlohmann/json_fwd.hpp>

#include "PatchDefinition.h"

class SampleLoader;
class Voice;
using json = nlohmann::json;

constexpr std::string_view SAMPLE_MAP_FILE_PATH = "Assets/Samples/instrument_oscillator_map.json";
constexpr std::string_view ENVELOPE_MAP_FILE_PATH = "Assets/Samples/instrument_envelope_map.json";

class InstrumentRegistry {
private:
    float sample_rate;
    std::array<PatchDefinition, 128> melodic_patches;
    std::array<uint8_t, 128> melodic_patch_aliases;
    std::array<PatchDefinition, 128> drum_patches;
    std::array<uint8_t, 128> drum_patch_aliases;

    void parse_oscillator_map_json(const json& json_data, std::array<PatchDefinition, 128>& patches, std::array<uint8_t, 128>& aliases);
    void parse_oscillator_config(const json& config, PatchDefinition* patch, SampleLoader* loader);
    static void parse_sample_zone_config(const json& config, PatchDefinition* patch, SampleLoader* loader);

    static void parse_envelope_map_json(const json& json_data, std::array<PatchDefinition, 128>& patches, std::array<uint8_t, 128>& aliases);
    static void parse_envelope_config(const json& config, PatchDefinition* patch);

    void init_sample_instruments();
    void set_fallbacks();

public:
    explicit InstrumentRegistry(float sample_rate = 48000.0f);

    [[nodiscard]] const PatchDefinition* get_melodic_patch_config(uint8_t patch_id) const;
    [[nodiscard]] const PatchDefinition* get_drum_patch_config(uint8_t pitch_key) const;
};


#endif //MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H