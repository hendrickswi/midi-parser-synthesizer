#ifndef MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H
#define MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H
#include <array>
#include <cstdint>
#include <set>
#include <unordered_map>
#include <nlohmann/json_fwd.hpp>

#include "OscillatorParsingType.h"
#include "PatchDefinition.h"

class SampleLoader;
class Voice;
using json = nlohmann::json;

constexpr std::string_view SAMPLE_MAP_FILE_PATH = "Assets/Samples/instrument_oscillator_map.json";
constexpr std::string_view ENVELOPE_MAP_FILE_PATH = "Assets/Samples/instrument_envelope_map.json";
constexpr uint8_t NUM_PATCHES = 128;

class InstrumentRegistry {
private:
    float sample_rate;
    std::array<PatchDefinition, NUM_PATCHES> melodic_patches;
    std::array<uint8_t, NUM_PATCHES> melodic_patch_aliases;
    std::array<PatchDefinition, NUM_PATCHES> drum_patches;
    std::array<uint8_t, NUM_PATCHES> drum_patch_aliases;

    // The top level parsers ("routers")
    static void parse_oscillator_map_json(
        const json &json_data,
        float target_sample_rate,
        std::array<PatchDefinition, NUM_PATCHES> *patches,
        std::array<uint8_t, NUM_PATCHES> *aliases,
        const std::set<uint8_t> &patch_numbers = std::set<uint8_t>()
    );
    static void parse_envelope_map_json(
        const json &json_data,
        float target_sample_rate,
        std::array<PatchDefinition, NUM_PATCHES> *patches,
        const std::set<uint8_t> &patch_numbers = std::set<uint8_t>()
    );

    // The intra-patch parsers ("workers")
    static void parse_oscillator_config(const json& config, float target_sample_rate, PatchDefinition* patch, SampleLoader* loader);
    static void parse_sample_zone_config(const json& config, PatchDefinition* patch, SampleLoader* loader);
    static void parse_svf_config(const json& config, float target_sample_rate, PatchDefinition* patch, SampleLoader* loader);
    static void parse_envelope_config(const json& config, float target_sample_rate, PatchDefinition* patch);

    void set_fallbacks();

public:
    explicit InstrumentRegistry(float sample_rate = 48000.0f);

    float get_sample_rate() const;
    void set_sample_rate(float sample_rate);
    void load_patches(const std::set<uint8_t>& melodic_patch_numbers, const std::set<uint8_t>& drum_patch_numbers);
    void clear_patches();

    [[nodiscard]] const PatchDefinition* get_melodic_patch_config(uint8_t patch_id) const;
    [[nodiscard]] const PatchDefinition* get_drum_patch_config(uint8_t pitch_key) const;
};


#endif //MIDI_PARSERSYNTHESIZER_INSTRUMENTREGISTRY_H