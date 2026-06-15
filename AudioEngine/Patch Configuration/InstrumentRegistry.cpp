#include "InstrumentRegistry.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "EnvelopeParsingType.h"
#include "OscillatorParsingType.h"
#include "../Synthesizer/Sample Loading/Sample.h"
#include "../Synthesizer/Sample Loading/SampleLoader.h"
#include "../Synthesizer/Voices/Voice.h"
#include "../../DirectoryManipulator.h"

void InstrumentRegistry::parse_oscillator_map_json(const json& json_data, std::array<PatchDefinition, 128>& patches, std::array<uint8_t, 128>& aliases) {
    SampleLoader loader = SampleLoader();

    for (const auto& [patch_id_str, patch_data] : json_data.items()) {
        int patch_id = 0;
        try {
            patch_id = std::stoi(patch_id_str);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "WARNING: Expected an integer patch key string, but found: \""
                      << patch_id_str << "\". Skipping entry..." << std::endl;
            continue;
        }
        catch (const std::out_of_range& e) {
            std::cerr << "WARNING: Patch key \"" << patch_id_str << "\" is out of integer bounds. Skipping..." << std::endl;
            continue;
        }

        if (patch_id < 0 || patch_id >= 128) {
            std::cerr << "WARNING: Patch index " << patch_id << " out of bounds. Skipping..." << std::endl;
            continue;
        }

        // Prevent loading the same samples and wasting memory
        if (patch_data.contains("copy_from")) {
            const auto& raw_data = patch_data["copy_from"];
            uint8_t source_id = raw_data.is_string() ?
                std::stoi(raw_data.get<std::string>()) :
                raw_data.get<uint8_t>();
            aliases[patch_id] = source_id;
            continue;
        }

        parse_oscillator_config(patch_data, &patches[patch_id], &loader);
    }
}

void InstrumentRegistry::parse_oscillator_config(const json& config, PatchDefinition* patch, SampleLoader* loader) {
    OscillatorParsingType type = config.value("oscillator_type", OscillatorParsingType::UNKNOWN);

    switch (type) {
        case OscillatorParsingType::SAMPLE : {
            patch->oscillator_type = OscillatorType::SAMPLE;
            for (const auto& zone : config.value("zones", json::array())) {
                parse_sample_zone_config(zone, patch, loader);
            }
            patch->oscillator_initialized = true;
            break;
        }
        case OscillatorParsingType::SQUARE: {
            patch->oscillator_type = OscillatorType::SQUARE;
            patch->square_oscillator_params = SquareOscillatorParams(440.0f, sample_rate);
            patch->oscillator_initialized = true;
            break;
        }
        case OscillatorParsingType::SAWTOOTH: {
            patch->oscillator_type = OscillatorType::SAWTOOTH;
            patch->sawtooth_oscillator_params = SawtoothOscillatorParams(440.0f, sample_rate);
            patch->oscillator_initialized = true;
            break;
        }
        case OscillatorParsingType::TRIANGLE: {
            patch->oscillator_type = OscillatorType::TRIANGLE;
            patch->triangle_oscillator_params = TriangleOscillatorParams(440.0f, sample_rate);
            patch->oscillator_initialized = true;
            break;
        }
        case OscillatorParsingType::SINE: {
            patch->oscillator_type = OscillatorType::SINE;
            patch->sine_oscillator_params = SineOscillatorParams(440.0f, sample_rate);
            patch->oscillator_initialized = true;
            break;
        }
        case OscillatorParsingType::NOISE: {
            patch->oscillator_type = OscillatorType::NOISE;
            patch->noise_oscillator_params = NoiseOscillatorParams();
            patch->oscillator_initialized = true;
            break;
        }
        case OscillatorParsingType::COMPOSITE: {
            patch->oscillator_type = OscillatorType::COMPOSITE;
            if (config.contains("composite_params")) {
                const auto& composite_params = config["composite_params"];

                int num_children = composite_params.value("num_children", 0);
                if (num_children > 0 && num_children <= MAX_CHILDREN_IN_COMPOSITE) {
                    std::vector<OscillatorType> child_types = std::vector<OscillatorType>();
                    for (const auto& raw_string_type : composite_params.value("child_types", json::array())) {
                        // Translate from raw string to parsing enum
                        OscillatorParsingType parsing_type = raw_string_type.get<OscillatorParsingType>();

                        // Then translate from parsing enum to real OscillatorType
                        switch (parsing_type) {
                            // Only possible types are the ones available in CompositeOscillatorNode.h
                            case OscillatorParsingType::SQUARE : {
                                child_types.push_back(OscillatorType::SQUARE);
                                break;
                            }
                            case OscillatorParsingType::SAWTOOTH : {
                                child_types.push_back(OscillatorType::SAWTOOTH);
                                break;
                            }
                            case OscillatorParsingType::TRIANGLE : {
                                child_types.push_back(OscillatorType::TRIANGLE);
                                break;
                            }
                            case OscillatorParsingType::SINE : {
                                child_types.push_back(OscillatorType::SINE);
                                break;
                            }
                            case OscillatorParsingType::NOISE : {
                                child_types.push_back(OscillatorType::NOISE);
                                break;
                            }
                            default : {
                                std::cerr << "WARNING: Invalid child oscillator type for composite oscillator found in json data. "
                                    "Defaulting to sine oscillator fallback." << std::endl;
                                break;
                            }
                        }
                    }

                    std::vector<float> mix_volumes;
                    mix_volumes.reserve(num_children);
                    if (mix_volumes.max_size() > 0) {
                        for (const auto& raw_float_volume : composite_params.value("mix_volumes", json::array())) {
                            mix_volumes.push_back(raw_float_volume.get<float>());
                        }
                    }

                    std::vector<float> child_frequency_ratios;
                    child_frequency_ratios.reserve(num_children);
                    if (child_frequency_ratios.max_size() > 0) {
                        for (const auto& raw_float_ratio : composite_params.value("child_frequency_ratios", json::array())) {
                            child_frequency_ratios.push_back(raw_float_ratio.get<float>());
                        }
                    }

                    patch->composite_oscillator_params = CompositeOscillatorParams(
                        440.0f,
                        sample_rate,
                        child_types,
                        mix_volumes,
                        child_frequency_ratios,
                        num_children
                    );
                    patch->oscillator_initialized = true;
                }
                else {
                    std::cerr << "WARNING: Improperly formatted composite oscillator definition found! Falling back to sine oscillator" << std::endl;
                }
            }
            else {
                std::cerr << "WARNING: Improperly formatted composite oscillator definition found! Falling back to sine oscillator" << std::endl;
            }
            break;
        }
        case OscillatorParsingType::VIBRATO : {
            patch->oscillator_decorator_type = OscillatorDecoratorType::VIBRATO;
            patch->vibrato_decorator_params.sample_rate = config.value("sample_rate", 48000.0f);
            patch->vibrato_decorator_params.base_hz = config.value("base_hz", 440.0f);
            patch->vibrato_decorator_params.speed_hz = config.value("speed_hz", 5.0f);
            patch->vibrato_decorator_params.depth = config.value("depth", 0.5f);

            if (config.contains("base_oscillator")) {
                parse_oscillator_config(config["base_oscillator"], patch, loader);
            }
            else {
                // Continue with a fallback sine oscillator
                std::cerr << "WARNING: No base oscillator specified for vibrato decorator. "
                             "Sine oscillator fallback will be used." << std::endl;
                patch->oscillator_type = OscillatorType::SINE;
                patch->sine_oscillator_params = SineOscillatorParams(440.0f, sample_rate);
                patch->oscillator_initialized = true;
            }
            break;
        }
        case OscillatorParsingType::LOWPASS : {
            patch->oscillator_decorator_type = OscillatorDecoratorType::LOWPASS;
            patch->low_pass_filter_params.sample_rate = config.value("sample_rate", 48000.0f);
            patch->low_pass_filter_params.cutoff_hz = config.value("cutoff_hz", 2000.0f);
            patch->low_pass_filter_params.resonance = config.value("resonance", 0.707f);

            if (config.contains("base_oscillator")) {
                parse_oscillator_config(config["base_oscillator"], patch, loader);
            }
            else {
                // Continue with a fallback sine oscillator
                std::cerr << "WARNING: No base oscillator specified for lowpass decorator. "
                             "Sine oscillator fallback will be used." << std::endl;
                patch->oscillator_type = OscillatorType::SINE;
                patch->sine_oscillator_params = SineOscillatorParams(440.0f, sample_rate);
                patch->oscillator_initialized = true;
            }
            break;
        }
        case OscillatorParsingType::BANDPASS : {
            patch->oscillator_decorator_type = OscillatorDecoratorType::BANDPASS;
            patch->band_pass_filter_params.sample_rate = config.value("sample_rate", 48000.0f);
            patch->band_pass_filter_params.cutoff_hz = config.value("cutoff_hz", 2000.0f);
            patch->band_pass_filter_params.resonance = config.value("resonance", 0.707f);

            if (config.contains("base_oscillator")) {
                parse_oscillator_config(config["base_oscillator"], patch, loader);
            }
            else {
                // Continue with a fallback sine oscillator
                std::cerr << "WARNING: No base oscillator specified for lowpass decorator. "
                             "Sine oscillator fallback will be used." << std::endl;
                patch->oscillator_type = OscillatorType::SINE;
                patch->sine_oscillator_params = SineOscillatorParams(440.0f, sample_rate);
                patch->oscillator_initialized = true;
            }
            break;
        }
        case OscillatorParsingType::HIGHPASS : {
            patch->oscillator_decorator_type = OscillatorDecoratorType::HIGHPASS;
            patch->high_pass_filter_params.sample_rate = config.value("sample_rate", 48000.0f);
            patch->high_pass_filter_params.cutoff_hz = config.value("cutoff_hz", 2000.0f);
            patch->high_pass_filter_params.resonance = config.value("resonance", 0.707f);

            if (config.contains("base_oscillator")) {
                parse_oscillator_config(config["base_oscillator"], patch, loader);
            }
            else {
                // Continue with a fallback sine oscillator
                std::cerr << "WARNING: No base oscillator specified for lowpass decorator. "
                             "Sine oscillator fallback will be used." << std::endl;
                patch->oscillator_type = OscillatorType::SINE;
                patch->sine_oscillator_params = SineOscillatorParams(440.0f, sample_rate);
                patch->oscillator_initialized = true;
            }
        }
        case OscillatorParsingType::UNKNOWN :
        default : {
            std::cerr << "WARNING: Unknown oscillator type in JSON. Defaulting to SINE fallback." << std::endl;
            // Allow set_fallbacks() to dictate the default initialization
            break;
        }
    }
}

void InstrumentRegistry::parse_sample_zone_config(const json& config, PatchDefinition* patch, SampleLoader* loader) {
    if (config.is_null() || !config.contains("file") || !config.contains("base_frequency")) {
        std::cerr << "WARNING: Invalid parameter 'config' in parse_envelope_config(const json& config, PatchDefinition* patch_config): "
            << config.dump() << std::endl;
        return;
    }

    if (patch == nullptr) {
        std::cerr << "WARNING: nullptr for parameter 'patch_config' passed into parse_envelope_config(const json& config, PatchDefinition* patch_config). " << std::endl;
        return;
    }

    std::string file_path = config["file"];
    float base_freq = config["base_frequency"];
    uint8_t min_pitch = config.value("min_pitch", 0);
    uint8_t max_pitch = config.value("max_pitch", 127);
    uint8_t min_velocity = config.value("min_velocity", 0);
    uint8_t max_velocity = config.value("max_velocity", 127);

    auto raw_sample = loader->load_wav_mono(file_path);
    if (raw_sample.audio_buffer.empty()) {
        std::cerr << "WARNING: Audio buffer is completely empty for file: " << file_path << std::endl;
    }

    patch->sample_oscillator_params.samples.push_back(Sample(
        raw_sample.audio_buffer, raw_sample.sample_rate, base_freq, min_pitch, max_pitch, min_velocity, max_velocity)
    );
}

void InstrumentRegistry::parse_envelope_map_json(const json& json_data, std::array<PatchDefinition, 128>& patches, std::array<uint8_t, 128>& aliases) {
    for (const auto& [patch_id_str, patch_data] : json_data.items()) {
        int patch_id = 0;
        try {
            patch_id = std::stoi(patch_id_str);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "WARNING: Expected an integer patch key string, but found: \""
                      << patch_id_str << "\". Skipping entry..." << std::endl;
            continue;
        }
        catch (const std::out_of_range& e) {
            std::cerr << "WARNING: Patch key \"" << patch_id_str << "\" is out of integer bounds. Skipping..." << std::endl;
            continue;
        }

        if (patch_id < 0 || patch_id >= 128) {
            std::cerr << "WARNING: Patch index " << patch_id << " out of bounds. Skipping..." << std::endl;
            continue;
        }

        PatchDefinition* patch = &patches[patch_id];

        patch->is_one_shot = patch_data.value("one_shot", false);
        if (patch_data.contains("envelope")) {
            parse_envelope_config(patch_data["envelope"], patch);
            patch->envelope_initialized = true;
        }
    }
}

void InstrumentRegistry::parse_envelope_config(const json& config, PatchDefinition* patch) {
    if (config.is_null() || !config.contains("type")) {
        std::cerr << "WARNING: Invalid parameter 'config' in parse_envelope_config(const json& config, PatchDefinition* patch_config): "
            << config.dump() << std::endl;
        return;
    }

    if (patch == nullptr) {
        std::cerr << "WARNING: nullptr for parameter 'patch_config' passed into parse_envelope_config(const json& config, PatchDefinition* patch_config). " << std::endl;
        return;
    }

    ParsingEnvelopeType type = config["type"];
    switch (type) {
        case ParsingEnvelopeType::ADSR : {
            patch->envelope_type = EnvelopeType::ADSR;
            patch->adsr_envelope_params.attack_time = config.value("attack_time", 0.005f);
            patch->adsr_envelope_params.attack_max = config.value("attack_max", 1.0f);
            patch->adsr_envelope_params.decay_time = config.value("decay_time", 1.0f);
            patch->adsr_envelope_params.sustain_level = config.value("sustain_level", 0.025f);
            patch->adsr_envelope_params.release_time = config.value("release_time", 0.2f);
            patch->adsr_envelope_params.release_min = config.value("release_min", 0.0f);
            break;
        }
        case ParsingEnvelopeType::ADR : {
            patch->envelope_type = EnvelopeType::ADR;
            patch->adr_envelope_params.attack_time = config.value("attack_time", 0.005f);
            patch->adr_envelope_params.attack_max = config.value("attack_max", 1.0f);
            patch->adr_envelope_params.decay_time = config.value("decay_time", 0.1f);
            patch->adr_envelope_params.release_time = config.value("release_time", 0.1f);
            patch->adr_envelope_params.release_max = config.value("release_time", 0.1f);
            patch->adr_envelope_params.release_min = config.value("release_min", 0.0f);
            break;
        }
        case ParsingEnvelopeType::TREMOLO : {
            patch->envelope_decorator_type = EnvelopeDecoratorType::TREMOLO;
            patch->tremolo_decorator_params.sample_rate = config.value("sample_rate", 48000.0f);
            patch->tremolo_decorator_params.speed_hz = config.value("speed_hz", 5.0f);
            patch->tremolo_decorator_params.depth = config.value("depth", 0.5f);

            if (config.contains("base_envelope")) {
                parse_envelope_config(config["base_envelope"], patch);
            }
            else {
                // Continue with a fallback ADSR base envelope
                std::cerr << "WARNING: No base envelope specified for tremolo decorator. "
                             "ADSR envelope fallback will be used." << std::endl;
                patch->envelope_type = EnvelopeType::ADSR;
                patch->adsr_envelope_params = ADSREnvelopeParams(0.005f, 1.0f, 1.0f, 0.025f, 0.2f, 0.0f);
            }
            break;
        }
        case ParsingEnvelopeType::UNKNOWN :
        default: {
            std::cerr << "WARNING: Unknown ParsingEnvelopeType enum in selected json. "
                         "ADSR envelope fallback will be used." << std::endl;
            patch->envelope_type = EnvelopeType::ADSR;
            patch->adsr_envelope_params = ADSREnvelopeParams(0.005f, 1.0f, 1.0f, 0.025f, 0.2f, 0.0f);
            break;
        }
    }
}

void InstrumentRegistry::init_sample_instruments() {
    std::cout << "INFO: Starting sample loading and patch configuration..." << std::endl;

    std::ifstream samples_file = std::ifstream(get_sanitized_file_path(std::string(SAMPLE_MAP_FILE_PATH)));
    json samples_config;
    if (samples_file.is_open()) {
        samples_config = json::parse(samples_file);
    }
    else {
        std::cerr << "Failed to open instrument sample map file." << std::endl;
    }

    std::ifstream envelopes_file = std::ifstream(get_sanitized_file_path(std::string(ENVELOPE_MAP_FILE_PATH)));
    json envelopes_config;
    if (envelopes_file.is_open()) {
        envelopes_config = json::parse(envelopes_file);
    }
    else {
        std::cerr << "Failed to open instrument envelope map file." << std::endl;
    }

    // Load the sample data into memory
    if (!samples_config.is_null()) {
        if (samples_config.contains("melodic_instruments")) {
            parse_oscillator_map_json(samples_config["melodic_instruments"], melodic_patches, melodic_patch_aliases);
            std::cout << "INFO: Melodic instrument samples successfully loaded" << std::endl;
        }
        else {
            std::cerr << "WARNING: No melodic instrument samples found in instrument sample map file." << std::endl;
        }

        if (samples_config.contains("drum_instruments")) {
            parse_oscillator_map_json(samples_config["drum_instruments"], drum_patches, drum_patch_aliases);
            std::cout << "INFO: Drum instrument samples successfully loaded" << std::endl;
        }
        else {
            std::cerr << "WARNING: No drum instrument samples found in instrument sample map file." << std::endl;
        }
    }
    else {
        std::cerr << "WARNING: Parsed json samples config is null. "
            "Instruments will use the fallback sine oscillator." << std::endl;
    }


    // Load the envelope data into memory
    if (!envelopes_config.is_null()) {
        if (envelopes_config.contains("melodic_instruments")) {
            parse_envelope_map_json(envelopes_config["melodic_instruments"], melodic_patches, melodic_patch_aliases);
            std::cout << "INFO: Melodic instrument envelopes successfully loaded" << std::endl;
        }
        else {
            std::cerr << "WARNING: No melodic instrument envelope configs found in instrument envelope map file." << std::endl;
        }

        if (envelopes_config.contains("drum_instruments")) {
            parse_envelope_map_json(envelopes_config["drum_instruments"], drum_patches, drum_patch_aliases);
            std::cout << "INFO: Drum instrument envelopes successfully loaded" << std::endl;
        }
        else {
            std::cerr << "WARNING: No drum instrument envelope configs found in instrument envelope map file." << std::endl;
        }
    }
    else {
        std::cerr << "WARNING: Parsed json envelopes config is null. "
            "Instruments will use the fallback ADSR envelope." << std::endl;
    }
}

void InstrumentRegistry::set_fallbacks() {
    for (auto& patch : melodic_patches) {
        if (!patch.oscillator_initialized) {
            patch.oscillator_type = OscillatorType::SINE;
            patch.sine_oscillator_params = SineOscillatorParams(440.0f, sample_rate);
            patch.oscillator_initialized = true;
            patch.oscillator_decorator_type = OscillatorDecoratorType::NONE;
        }

        if (!patch.envelope_initialized) {
            patch.envelope_type = EnvelopeType::ADSR;
            patch.adsr_envelope_params = ADSREnvelopeParams(
                sample_rate, 0.005f, 0.6f, 0.2f, 0.25f, 0.1f, 0.0f
            );
            patch.envelope_decorator_type = EnvelopeDecoratorType::NONE;

            patch.is_one_shot = false;
            patch.envelope_initialized = true;
        }
    }

    for (auto& patch : drum_patches) {
        if (!patch.oscillator_initialized) {
            patch.oscillator_type = OscillatorType::NOISE;
            patch.noise_oscillator_params = NoiseOscillatorParams();
            patch.oscillator_initialized = true;
        }

        if (!patch.envelope_initialized) {
            patch.envelope_type = EnvelopeType::ADR;
            patch.adr_envelope_params = ADREnvelopeParams(
                sample_rate, 0.001f, 0.4f, 0.05f, 0.05f, 0.1f, 0.0f
            );

            patch.is_one_shot = true;
            patch.envelope_initialized = true;
        }
    }
}

InstrumentRegistry::InstrumentRegistry(float sample_rate) { // NOLINT
    this->sample_rate = sample_rate;

    for (uint8_t i = 0; i < 128; ++i) {
        melodic_patch_aliases[i] = i;
        drum_patch_aliases[i] = i;
    }

    init_sample_instruments();
    set_fallbacks();
}

const PatchDefinition* InstrumentRegistry::get_melodic_patch_config(uint8_t patch_id) const {
    return &melodic_patches[melodic_patch_aliases[patch_id]];
}

const PatchDefinition* InstrumentRegistry::get_drum_patch_config(uint8_t pitch_key) const {
    return &drum_patches[drum_patch_aliases[pitch_key]];
}
