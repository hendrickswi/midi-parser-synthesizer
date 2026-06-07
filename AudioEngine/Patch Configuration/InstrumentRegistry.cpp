#include "InstrumentRegistry.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "../Synthesizer/Envelopes/EnvelopeParsingType.h"
#include "../Synthesizer/Sample Loading/Sample.h"
#include "../Synthesizer/Sample Loading/SampleLoader.h"
#include "../Synthesizer/Voices/Voice.h"
#include "../../DirectoryManipulator.h"

using json = nlohmann::json;

void parse_envelope_config(const json& config, PatchDefinition* patch_config) {
    if (config.is_null() || !config.contains("type")) {
        std::cerr << "WARNING: Invalid envelope config in parse_envelope_config(...): " << config.dump() << std::endl;
    }

    if (patch_config == nullptr) {
        std::cerr << "WARNING: null pointer for parameter patch_config passed into parse_envelope_config(...). " << std::endl;
        return;
    }

    ParsingEnvelopeType type = config["type"];
    switch (type) {
        case ParsingEnvelopeType::ADSR : {
            patch_config->envelope_type = EnvelopeType::ADSR;
            patch_config->adsr_envelope_params.attack_time = config.value("attack_time", 0.005f);
            patch_config->adsr_envelope_params.attack_max = config.value("attack_max", 1.0f);
            patch_config->adsr_envelope_params.decay_time = config.value("decay_time", 1.0f);
            patch_config->adsr_envelope_params.sustain_level = config.value("sustain_level", 0.025f);
            patch_config->adsr_envelope_params.release_time = config.value("release_time", 0.2f);
            patch_config->adsr_envelope_params.release_min = config.value("release_min", 0.0f);
            break;
        }
        case ParsingEnvelopeType::ADR : {
            patch_config->envelope_type = EnvelopeType::ADR;
            patch_config->adr_envelope_params.attack_time = config.value("attack_time", 0.005f);
            patch_config->adr_envelope_params.attack_max = config.value("attack_max", 1.0f);
            patch_config->adr_envelope_params.decay_time = config.value("decay_time", 0.1f);
            patch_config->adr_envelope_params.release_time = config.value("release_time", 0.1f);
            patch_config->adr_envelope_params.release_max = config.value("release_time", 0.1f);
            patch_config->adr_envelope_params.release_min = config.value("release_min", 0.0f);
            break;
        }
        case ParsingEnvelopeType::TREMOLO : {
            patch_config->envelope_decorator_type = EnvelopeDecoratorType::TREMOLO;
            patch_config->tremolo_decorator_params.speed_hz = config.value("speed_hz", 5.0f);
            patch_config->tremolo_decorator_params.depth = config.value("depth", 0.5f);

            if (config.contains("base_envelope")) {
                parse_envelope_config(config["base_envelope"], patch_config);
            }
            else {
                // Continue with a fallback ADSR base envelope
                std::cerr << "WARNING: No base envelope specified for tremolo decorator. "
                             "ADSR envelope fallback will be used." << std::endl;
                patch_config->envelope_type = EnvelopeType::ADSR;
                patch_config->adsr_envelope_params = ADSREnvelopeParams(0.005f, 1.0f, 1.0f, 0.025f, 0.2f, 0.0f);
            }
            break;
        }
        case ParsingEnvelopeType::UNKNOWN :
        default: {
            std::cerr << "WARNING: Unknown ParsingEnvelopeType enum in selected json. "
                         "ADSR envelope fallback will be used." << std::endl;
            patch_config->envelope_type = EnvelopeType::ADSR;
            patch_config->adsr_envelope_params = ADSREnvelopeParams(0.005f, 1.0f, 1.0f, 0.025f, 0.2f, 0.0f);
            break;
        }
    }
}

void InstrumentRegistry::init_samples() {
    SampleLoader loader;
    std::ifstream file = std::ifstream(get_sanitized_file_path("Assets/Samples/instrument_map.json"));
    if (!file.is_open()) {
        std::cerr << "Failed to open instrument map file. Samples loading will be skipped." << std::endl;
        return;
    }

    json config = json::parse(file);

    // Loading melodic samples
    for (const auto& [patch_id_str, melodic_patch_data] : config["melodic_instruments"].items()) {
        int patch_id = std::stoi(patch_id_str);

        // Prevent loading the same samples and wasting memory
        if (melodic_patch_data.contains("copy_from")) {
            uint8_t source_id = melodic_patch_data["copy_from"];
            melodic_patch_aliases[patch_id] = source_id;
            continue;
        }

        PatchDefinition* patch = &melodic_patches[patch_id];

        patch->oscillator_type = OscillatorType::SAMPLE;
        patch->is_one_shot = melodic_patch_data.value("one_shot", false);

        if (melodic_patch_data.contains("envelope")) {
            parse_envelope_config(melodic_patch_data["envelope"], patch);
        }

        for (const auto& zone : melodic_patch_data.value("zones", json::array())) {
            std::string file_path = zone["file"];
            float base_freq = zone["base_frequency"];
            uint8_t min_pitch = zone["min_pitch"];
            uint8_t max_pitch = zone["max_pitch"];
            uint8_t min_velocity = zone.value("min_velocity", 0);
            uint8_t max_velocity = zone.value("max_velocity", 127);

            auto raw_sample = loader.load_wav_mono(file_path);
            if (raw_sample.audio_buffer.empty()) {
                std::cerr << "WARNING: Audio buffer is completely empty for file: " << file_path << std::endl;
            }

            patch->sample_oscillator_params.samples.push_back(Sample(
                raw_sample.audio_buffer, raw_sample.sample_rate, base_freq, min_pitch, max_pitch, min_velocity, max_velocity)
            );
        }

        patch->is_initialized = true;
    }

    // Loading drum samples
    for (const auto& [drum_key_str, drum_patch_data] : config["drum_instruments"].items()) {
        int drum_key = std::stoi(drum_key_str);

        // Prevent loading the same samples and wasting memory
        if (drum_patch_data.contains("copy_from")) {
            uint8_t source_id = drum_patch_data["copy_from"];
            drum_patch_aliases[drum_key] = source_id;
            continue;
        }

        PatchDefinition* patch = &drum_patches[drum_key];

        patch->oscillator_type = OscillatorType::SAMPLE;
        patch->is_one_shot = drum_patch_data.value("one_shot", true);

        if (drum_patch_data.contains("envelope")) {
            parse_envelope_config(drum_patch_data["envelope"], patch);
        }

        for (const auto& zone : drum_patch_data.value("zones", json::array())) {
            std::string file_path = zone["file"];
            float base_freq = zone["base_frequency"];
            uint8_t min_velocity = zone.value("min_velocity", 0);
            uint8_t max_velocity = zone.value("max_velocity", 127);

            auto raw_sample = loader.load_wav_mono(file_path);
            if (raw_sample.audio_buffer.empty()) {
                std::cerr << "Audio buffer is completely empty for file: " << file_path << std::endl;
            }

            patch->sample_oscillator_params.samples.push_back(Sample(
                raw_sample.audio_buffer, raw_sample.sample_rate, base_freq, 0, 127, min_velocity, max_velocity)
            );
        }

        patch->is_initialized = true;
    }
}

void InstrumentRegistry::init_envelopes() {

}

void InstrumentRegistry::init_leads() {
    // Square lead
    auto& square_lead = melodic_patches[80];
    square_lead.oscillator_type = OscillatorType::SQUARE;
    square_lead.square_oscillator_params = SquareOscillatorParams(440.0f, sample_rate);
    square_lead.envelope_type = EnvelopeType::ADSR;
    square_lead.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.005f, 0.6f, 0.2f, 0.25f, 0.1f, 0.0f
    );
    square_lead.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    square_lead.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    square_lead.is_one_shot = false;
    square_lead.is_initialized = true;

    // Sawtooth lead
    auto& sawtooth_lead = melodic_patches[81];
    sawtooth_lead.oscillator_type = OscillatorType::SAWTOOTH;
    sawtooth_lead.sawtooth_oscillator_params = SawtoothOscillatorParams(440.0f, sample_rate);
    sawtooth_lead.envelope_type = EnvelopeType::ADSR;
    sawtooth_lead.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.005, 0.6f, 0.2f, 0.25f, 0.1f, 0.0f
    );
    sawtooth_lead.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    sawtooth_lead.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    sawtooth_lead.is_one_shot = false;
    sawtooth_lead.is_initialized = true;

    // Calliope lead
    auto& calliope_lead = melodic_patches[82];
    calliope_lead.oscillator_type = OscillatorType::TRIANGLE;
    calliope_lead.triangle_oscillator_params = TriangleOscillatorParams(440.0f, sample_rate);
    calliope_lead.envelope_type = EnvelopeType::ADSR;
    calliope_lead.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.02f, 0.6f, 0.1f, 0.7f, 0.2f, 0.0f
    );
    calliope_lead.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    calliope_lead.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    calliope_lead.is_one_shot = false;
    calliope_lead.is_initialized = true;

    // Chiff lead
    auto& chiff_lead = melodic_patches[83];
    chiff_lead.oscillator_type = OscillatorType::SINE;
    chiff_lead.sine_oscillator_params = SineOscillatorParams(440.0f, sample_rate);
    chiff_lead.envelope_type = EnvelopeType::ADSR;
    chiff_lead.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.005f, 0.6f, 0.15f, 0.4f, 0.15f, 0.0f
    );
    chiff_lead.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    chiff_lead.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    chiff_lead.is_one_shot = false;
    chiff_lead.is_initialized = true;

    // Charang lead
    auto& charang_lead = melodic_patches[84];
    charang_lead.oscillator_type = OscillatorType::SQUARE;
    charang_lead.square_oscillator_params = SquareOscillatorParams(440.0f, sample_rate);
    charang_lead.envelope_type = EnvelopeType::ADSR;
    charang_lead.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.005f, 0.6f, 0.3f, 0.2f, 0.15f, 0.0f
    );
    charang_lead.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    charang_lead.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    charang_lead.is_one_shot = false;
    charang_lead.is_initialized = true;

    // Voice lead
    auto& voice_lead = melodic_patches[85];
    voice_lead.oscillator_type = OscillatorType::TRIANGLE;
    voice_lead.triangle_oscillator_params = TriangleOscillatorParams(440.0f, sample_rate);
    voice_lead.envelope_type = EnvelopeType::ADSR;
    voice_lead.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.06f, 0.6f, 0.1f, 0.7f, 0.25f, 0.0f
    );
    voice_lead.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    voice_lead.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    voice_lead.is_one_shot = false;
    voice_lead.is_initialized = true;

    // Fifths lead
    auto& fifths_lead = melodic_patches[86];
    fifths_lead.oscillator_type = OscillatorType::COMPOSITE;
    fifths_lead.composite_oscillator_params = CompositeOscillatorParams(
        440.0f,
        sample_rate,
        { OscillatorType::SAWTOOTH, OscillatorType::SAWTOOTH },
        { 0.6f, 0.4f },
        { 1.0f, std::pow(2.0f, 7.0f / 12.0f) },
        2
    );
    fifths_lead.envelope_type = EnvelopeType::ADSR;
    fifths_lead.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.005f, 0.6f, 0.15f, 0.4f, 0.15f, 0.0f
    );
    fifths_lead.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    fifths_lead.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    fifths_lead.is_one_shot = false;
    fifths_lead.is_initialized = true;

    // Bass + lead
    auto& bass_lead = melodic_patches[87];
    bass_lead.oscillator_type = OscillatorType::COMPOSITE;
    bass_lead.composite_oscillator_params = CompositeOscillatorParams(
        440.0f,
        sample_rate,
        { OscillatorType::SAWTOOTH, OscillatorType::SQUARE },
        { 0.45f, 0.55f },
        { 1.0f, 0.5f },
        2
    );
    bass_lead.envelope_type = EnvelopeType::ADSR;
    bass_lead.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.005f, 0.8f, 0.2f, 0.5f, 0.1f, 0.0f
    );
    bass_lead.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    bass_lead.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    bass_lead.is_one_shot = false;
    bass_lead.is_initialized = true;
}

void InstrumentRegistry::init_pads() {
    // Pad 1 (New age)
    auto& new_age_pad = melodic_patches[88];
    new_age_pad.oscillator_type = OscillatorType::COMPOSITE;
    new_age_pad.composite_oscillator_params = CompositeOscillatorParams(
        440.0f,
        sample_rate,
        { OscillatorType::TRIANGLE, OscillatorType::SINE, OscillatorType::SAWTOOTH },
        { 0.5f, 0.2f, 0.2f },
        { 1.0f, 2.0f, 1.003f },
        3
    );
    new_age_pad.envelope_type = EnvelopeType::ADSR;
    new_age_pad.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.2f, 1.0f, 0.001f, 1.0f, 0.2f, 0.0f
    );
    new_age_pad.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    new_age_pad.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    new_age_pad.is_one_shot = false;
    new_age_pad.is_initialized = true;

    // Pad 2 (Warm)
    auto& warm_pad = melodic_patches[89];
    warm_pad.oscillator_type = OscillatorType::COMPOSITE;
    warm_pad.composite_oscillator_params = CompositeOscillatorParams(
        440.0f,
        sample_rate,
        { OscillatorType::SAWTOOTH, OscillatorType::SAWTOOTH, OscillatorType::SAWTOOTH },
        { 0.34f, 0.33f, 0.33f },
        { 1.0f, 0.996f, 1.004f },
        3
    );
    warm_pad.envelope_type = EnvelopeType::ADSR;
    warm_pad.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.4f, 0.6f, 0.001f, 0.6f, 0.4f, 0.0f
    );
    warm_pad.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    warm_pad.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    warm_pad.is_one_shot = false;
    warm_pad.is_initialized = true;

    // Pad 3 (Polysynth)
    auto& polysynth_pad = melodic_patches[90];
    polysynth_pad.oscillator_type = OscillatorType::COMPOSITE;
    polysynth_pad.composite_oscillator_params = CompositeOscillatorParams(
        440.0f,
        sample_rate,
        { OscillatorType::SQUARE, OscillatorType::SAWTOOTH, OscillatorType::SAWTOOTH },
        { 0.4f, 0.3f, 0.3f },
        { 1.0f, 1.0f, 1.003f },
        3
    );
    polysynth_pad.envelope_type = EnvelopeType::ADSR;
    polysynth_pad.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.05f, 0.6f, 0.3f, 0.6f, 0.3f, 0.0f
    );
    polysynth_pad.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    polysynth_pad.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    polysynth_pad.is_one_shot = false;
    polysynth_pad.is_initialized = true;

    // Pad 4 (Choir)
    auto& choir_pad = melodic_patches[91];
    choir_pad.oscillator_type = OscillatorType::COMPOSITE;
    choir_pad.composite_oscillator_params = CompositeOscillatorParams(
        440.0f,
        sample_rate,
        { OscillatorType::TRIANGLE, OscillatorType::SAWTOOTH },
        { 0.4f, 0.3f },
        { 1.0f, 1.002f },
        2
    );
    choir_pad.envelope_type = EnvelopeType::ADSR;
    choir_pad.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.6f, 0.6f, 0.001f, 0.6f, 0.6f, 0.0f
    );
    choir_pad.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    choir_pad.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    choir_pad.is_one_shot = false;
    choir_pad.is_initialized = true;

    // Pad 5 (Bowed)
    auto& bowed_pad = melodic_patches[92];
    bowed_pad.oscillator_type = OscillatorType::COMPOSITE;
    bowed_pad.composite_oscillator_params = CompositeOscillatorParams(
        440.0f,
        sample_rate,
        { OscillatorType::TRIANGLE, OscillatorType::SAWTOOTH, OscillatorType::SAWTOOTH },
        { 0.4f, 0.3f, 0.3f },
        { 1.0f, 1.0f, 1.005f },
        3
    );
    bowed_pad.envelope_type = EnvelopeType::ADSR;
    bowed_pad.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.5f, 0.6f, 0.1f, 0.4f, 0.5f, 0.0f
    );
    bowed_pad.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    bowed_pad.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    bowed_pad.is_one_shot = false;
    bowed_pad.is_initialized = true;

    // Pad 6 (Metallic)
    auto& metallic_pad = melodic_patches[93];
    metallic_pad.oscillator_type = OscillatorType::COMPOSITE;
    metallic_pad.composite_oscillator_params = CompositeOscillatorParams(
        440.0f,
        sample_rate,
        { OscillatorType::SQUARE, OscillatorType::SQUARE, OscillatorType::SAWTOOTH },
        { 0.4f, 0.4f, 0.2f },
        { 1.0f, 2.0f, 4.0f },
        3
    );
    metallic_pad.envelope_type = EnvelopeType::ADSR;
    metallic_pad.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.1f, 0.6f, 0.5f, 0.3f, 0.5f, 0.0f
    );
    metallic_pad.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    metallic_pad.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    metallic_pad.is_one_shot = false;
    metallic_pad.is_initialized = true;

    // Pad 7 (Halo)
    auto& halo_pad = melodic_patches[94];
    halo_pad.oscillator_type = OscillatorType::COMPOSITE;
    halo_pad.composite_oscillator_params = CompositeOscillatorParams(
        440.0f,
        sample_rate,
        { OscillatorType::SINE, OscillatorType::SINE, OscillatorType::SAWTOOTH },
        { 0.5f, 0.3f, 0.2f },
        { 1.0f, 2.0f, 1.0f },
        3
    );
    halo_pad.envelope_type = EnvelopeType::ADSR;
    halo_pad.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.5f, 0.6f, 0.001f, 0.6f, 0.5f, 0.0f
    );
    halo_pad.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    halo_pad.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    halo_pad.is_one_shot = false;
    halo_pad.is_initialized = true;

    // Pad 8 (Sweep)
    auto& sweep_pad = melodic_patches[95];
    sweep_pad.oscillator_type = OscillatorType::COMPOSITE;
    sweep_pad.composite_oscillator_params = CompositeOscillatorParams(
        440.0f,
        sample_rate,
        { OscillatorType::SAWTOOTH, OscillatorType::SAWTOOTH },
        { 0.5f, 0.5f },
        { 0.992f, 1.008f },
        2
    );
    sweep_pad.envelope_type = EnvelopeType::ADSR;
    sweep_pad.adsr_envelope_params = ADSREnvelopeParams(
        sample_rate, 0.5f, 0.6f, 0.001f, 0.6f, 1.0f, 0.0f
    );
    sweep_pad.oscillator_decorator_type = OscillatorDecoratorType::NONE;
    sweep_pad.envelope_decorator_type = EnvelopeDecoratorType::NONE;
    sweep_pad.is_one_shot = false;
    sweep_pad.is_initialized = true;
}

void InstrumentRegistry::set_fallbacks() {
    for (auto& patch : melodic_patches) {
        if (!patch.is_initialized) {
            patch.oscillator_type = OscillatorType::SINE;
            patch.sine_oscillator_params = SineOscillatorParams(440.0f, sample_rate);
            patch.envelope_type = EnvelopeType::ADSR;
            patch.adsr_envelope_params = ADSREnvelopeParams(
                sample_rate, 0.005f, 0.6f, 0.2f, 0.25f, 0.1f, 0.0f
            );

            patch.oscillator_decorator_type = OscillatorDecoratorType::NONE;
            patch.envelope_decorator_type = EnvelopeDecoratorType::NONE;

            patch.is_one_shot = false;
            patch.is_initialized = true;
        }
    }

    for (auto& patch : drum_patches) {
        if (!patch.is_initialized) {
            patch.oscillator_type = OscillatorType::NOISE;
            patch.noise_oscillator_params = NoiseOscillatorParams();
            patch.envelope_type = EnvelopeType::ADR;
            patch.adr_envelope_params = ADREnvelopeParams(
                sample_rate, 0.001f, 0.6f, 0.05f, 0.05f, 0.1f, 0.0f
            );

            patch.oscillator_decorator_type = OscillatorDecoratorType::NONE;
            patch.envelope_decorator_type = EnvelopeDecoratorType::NONE;

            patch.is_one_shot = true;
            patch.is_initialized = true;
        }
    }
}

InstrumentRegistry::InstrumentRegistry(float sample_rate) { // NOLINT
    this->sample_rate = sample_rate;

    for (uint8_t i = 0; i < 128; ++i) {
        melodic_patches_aliases[i] = i;
        drum_patch_aliases[i] = i;
    }

    init_samples();
    init_leads();
    init_pads();
    set_fallbacks();
}

const PatchDefinition* InstrumentRegistry::get_melodic_patch_config(uint8_t patch_id) const {
    return &melodic_patches[melodic_patches_aliases[patch_id]];
}

const PatchDefinition* InstrumentRegistry::get_drum_patch_config(uint8_t pitch_key) const {
    return &drum_patches[drum_patch_aliases[pitch_key]];
}