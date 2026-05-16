#include "InstrumentRegistry.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "Envelopes/Base Implementations/ADR/ADREnvelope.h"
#include "Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
#include "Oscillators/Base Implementations/NoiseOscillator.h"
#include "Oscillators/Base Implementations/SampleOscillator.h"
#include "Oscillators/Base Implementations/SineOscillator.h"
#include "Sample Loading/Sample.h"
#include "Sample Loading/SampleLoader.h"
#include "Voices/Voice.h"

using json = nlohmann::json;

void InstrumentRegistry::init(float sample_rate) {
    this->sample_rate = sample_rate;
    init_samples();

    // Melodic sample handling
    for (int patch_id = 0; patch_id < 128; patch_id++) {
        if (melodic_samples[patch_id].empty()) continue;

        melodic_patch_factories[patch_id] = [this, patch_id](Voice* v, uint8_t pitch) {
            const auto& samples = this->melodic_samples[patch_id];

            // Find the appropriate sample to use for the requested pitch, if it exists
            const Sample* selected_sample = nullptr;
            for (const auto& sample : samples ) {
                if (pitch >= sample.min_pitch && pitch <= sample.max_pitch) {
                    selected_sample = &sample;
                    break;
                }
            }

            // Case: There is a sample available, but it is not in the correct pitch range
            if (selected_sample == nullptr) {
               selected_sample = &samples.front();
            }

            v->set_oscillator(std::make_unique<SampleOscillator>(
                &selected_sample->audio_buffer,
                selected_sample->base_frequency
            ));

            // TODO: Custom envelope values per patch via std::array<std::vector<EnvelopeData>> and JSON loading
            v->set_envelope(std::make_unique<ADSREnvelope>(
                this->sample_rate, 0.005f, 1.0f, 3.0f,
                0.025f, 0.2f, 0.0f
            ));
        };
    }

    // Drum sample handling
    for (int drum_key = 0; drum_key < 128; drum_key++) {
        if (drum_samples[drum_key].empty()) continue;

        drum_patch_factories[drum_key] = [this, drum_key](Voice* v, uint8_t pitch) {
            // Drums do not have multi-sampling, can just grab the first sample loaded
            const auto& selected_sample = this->drum_samples[drum_key].front();
            v->set_oscillator(std::make_unique<SampleOscillator>(
                &selected_sample.audio_buffer,
                selected_sample.base_frequency
            ));

            v->set_envelope(std::make_unique<ADREnvelope>(
                this->sample_rate, 0.005f, 1.0f, 0.1f, 0.1f, 0.1f, 0.0f
            ));
        };
    }

    for (auto& factory : melodic_patch_factories) {
        if (factory == nullptr) {
            // Fallback
            factory = [this](Voice* v, uint8_t pitch) {
                v->set_oscillator(std::make_unique<SineOscillator>(440.0f, this->sample_rate));
                v->set_envelope(std::make_unique<ADSREnvelope>(this->sample_rate, 0.005f, 1.0f, 3.0f,
                0.025f, 0.2f, 0.0f));
            };
        }
    }

    for (auto& factory : drum_patch_factories) {
        if (factory == nullptr) {
            // Fallback
            factory = [this](Voice* v, uint8_t pitch) {
                v->set_oscillator(std::make_unique<NoiseOscillator>());
                v->set_envelope(std::make_unique<ADREnvelope>(this->sample_rate, 0.005f, 1.0f,
                    0.1f, 0.1f, 0.1f, 0.0f));
            };
        }
    }
}

void InstrumentRegistry::init_samples() {
    SampleLoader loader;
    std::ifstream file = std::ifstream("Assets/instrument_map.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open instrument map file. Samples loading will be skipped." << std::endl;
        return;
    }
    json config = json::parse(file);

    // Loading melodic samples
    for (const auto& [patch_id_str, zones] : config["melodic_instruments"].items()) {
        int patch_id = std::stoi(patch_id_str);

        for (const auto& zone : zones) {
            std::string file_path = zone["file"];
            float base_freq = zone["base_frequency"];
            uint8_t min_p = zone["min_pitch"];
            uint8_t max_p = zone["max_pitch"];

            melodic_samples[patch_id].push_back(Sample(
                loader.load_wav_mono(file_path), base_freq, min_p, max_p
            ));
        }
    }

    // Loading drum samples
    for (const auto& [drum_key_str, drum_data] : config["drum_instruments"].items()) {
        int drum_key = std::stoi(drum_key_str);

        std::string file_path = drum_data["file"];
        float base_freq = drum_data["base_frequency"];

        drum_samples[drum_key].push_back(Sample(
            loader.load_wav_mono(file_path), base_freq, 0, 127
        ));
    }
}


InstrumentRegistry::InstrumentRegistry() { // NOLINT
    init();
}

InstrumentRegistry::InstrumentRegistry(float sample_rate) { // NOLINT
    init(sample_rate);
}

void InstrumentRegistry::configure_melodic_voice(std::uint8_t patch_id, Voice* voice, uint8_t pitch) {
    melodic_patch_factories[patch_id](voice, pitch);
}

void InstrumentRegistry::configure_drum_voice(std::uint8_t patch_id, Voice* voice, uint8_t pitch) {
    drum_patch_factories[patch_id](voice, pitch);
}