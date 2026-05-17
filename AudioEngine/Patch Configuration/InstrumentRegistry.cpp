#include "InstrumentRegistry.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "PatchFactory.h"
#include "../Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelope.h"
#include "../Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
#include "../Synthesizer/Oscillators/Base Implementations/NoiseOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/SampleOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/SawtoothOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/SineOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/SquareOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/TriangleOscillator.h"
#include "../Synthesizer/Sample Loading/Sample.h"
#include "../Synthesizer/Sample Loading/SampleLoader.h"
#include "../Synthesizer/Voices/Voice.h"

using json = nlohmann::json;

void InstrumentRegistry::init(float sample_rate) {
    this->sample_rate = sample_rate;
    init_samples();
    init_leads();

    // Melodic sample handling
    for (int patch_id = 0; patch_id < 128; patch_id++) {
        if (melodic_patch_configs[patch_id].samples.empty() || melodic_patch_factories[patch_id] != nullptr) continue;

        melodic_patch_factories[patch_id] = [this, patch_id](Voice* v, uint8_t pitch, uint8_t velocity) {
            /*
             * Sample selection logic:
             *      1st pass: Try to find a sample with matching pitch *and* velocity
             *      2nd pass: Try to find a sample with matching pitch and any velocity
             *      Else: Use the first sample in the list of available samples (confirmed to not be empty)
             */

            const auto& samples = this->melodic_patch_configs[patch_id].samples;
            const Sample* selected_sample = nullptr;

            for (const auto& sample : samples ) {
                if (pitch >= sample.min_pitch && pitch <= sample.max_pitch &&
                    velocity >= sample.min_velocity && velocity <= sample.max_velocity) {
                    selected_sample = &sample;
                    break;
                }
            }

            if (selected_sample == nullptr) {
                for (const auto& sample : samples ) {
                    if (pitch >= sample.min_pitch && pitch <= sample.max_pitch) {
                        selected_sample = &sample;
                        break;
                    }
                }
            }

            if (selected_sample == nullptr) {
               selected_sample = &samples.front();
            }

            v->set_oscillator(PatchFactory::create_sample_oscillator(
                &selected_sample->audio_buffer,
                selected_sample->base_frequency
            ));

            const auto& env_data = melodic_patch_configs[patch_id].envelope_data;
            v->set_envelope(PatchFactory::create_envelope(env_data, this->sample_rate));
        };
    }

    // Drum sample handling
    for (int drum_key = 0; drum_key < 128; drum_key++) {
        if (drum_patch_configs[drum_key].samples.empty()) continue;

        drum_patch_factories[drum_key] = [this, drum_key](Voice* v, uint8_t pitch, uint8_t velocity) {
            // Sample selection logic: Drums do not have multi-sampling,
            // therefore can just grab the first sample loaded.

            const auto& selected_sample = this->drum_patch_configs[drum_key].samples.front();
            v->set_oscillator(PatchFactory::create_sample_oscillator(
                &selected_sample.audio_buffer,
                selected_sample.base_frequency
            ));

            const auto& env_data = drum_patch_configs[drum_key].envelope_data;
            v->set_envelope(PatchFactory::create_envelope(env_data, this->sample_rate));
        };
    }

    for (auto& factory : melodic_patch_factories) {
        if (factory == nullptr) {
            // Fallback
            factory = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
                v->set_oscillator(PatchFactory::create_sine_oscillator(440.0, this->sample_rate));
                v->set_envelope(PatchFactory::create_adsr_envelope(this->sample_rate, 0.001f, 0.001f, 0.001f,
                0.001f, 0.001f, 0.0f));
            };
        }
    }

    for (auto& factory : drum_patch_factories) {
        if (factory == nullptr) {
            // Fallback
            factory = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
                v->set_oscillator(PatchFactory::create_noise_oscillator());
                v->set_envelope(PatchFactory::create_adr_envelope(this->sample_rate, 0.005f, 1.0f,
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
    for (const auto& [patch_id_str, melodic_patch_data] : config["melodic_instruments"].items()) {
        int patch_id = std::stoi(patch_id_str);

        melodic_patch_configs[patch_id].envelope_data = melodic_patch_data["envelope"];

        for (const auto& zone : melodic_patch_data.value("zones", json::array())) {
            std::string file_path = zone["file"];
            float base_freq = zone["base_frequency"];
            uint8_t min_pitch = zone["min_pitch"];
            uint8_t max_pitch = zone["max_pitch"];
            uint8_t min_velocity = zone.value("min_velocity", 0);
            uint8_t max_velocity = zone.value("max_velocity", 127);

            melodic_patch_configs[patch_id].samples.push_back(Sample(
                loader.load_wav_mono(file_path), base_freq, min_pitch, max_pitch, min_velocity, max_velocity)
            );
        }
    }

    // Loading drum samples
    for (const auto& [drum_key_str, drum_patch_data] : config["drum_instruments"].items()) {
        int drum_key = std::stoi(drum_key_str);

        drum_patch_configs[drum_key].envelope_data = drum_patch_data["envelope"];

        for (const auto& zone : drum_patch_data.value("zones", json::array())) {
            std::string file_path = zone["file"];
            float base_freq = zone["base_frequency"];
            uint8_t min_velocity = zone.value("min_velocity", 0);
            uint8_t max_pitch = zone.value("max_pitch", 127);

            drum_patch_configs[drum_key].samples.push_back(Sample(
                loader.load_wav_mono(file_path), base_freq, 0, 127, min_velocity, max_pitch)
            );
        }
    }
}

void InstrumentRegistry::init_leads() {
    melodic_patch_factories[80] = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
        v->set_oscillator(PatchFactory::create_square_oscillator(440.0f, sample_rate));
        v->set_envelope(std::make_unique<ADSREnvelope>(sample_rate, 0.005f, 1.0f, 0.2f,
            0.25f, 0.1f, 0.0f));
    };
    melodic_patch_factories[81] = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
        v->set_oscillator(PatchFactory::create_sawtooth_oscillator(440.0f, sample_rate));
        v->set_envelope(std::make_unique<ADSREnvelope>(sample_rate, 0.005f, 1.0f, 0.2f,
            0.25f, 0.1f, 0.0f));
    };
    // Can add more leads here later (e.g., calliope lead)
}


InstrumentRegistry::InstrumentRegistry() { // NOLINT
    init();
}

InstrumentRegistry::InstrumentRegistry(float sample_rate) { // NOLINT
    init(sample_rate);
}

void InstrumentRegistry::configure_melodic_voice(std::uint8_t patch_id, Voice* voice, uint8_t pitch, uint8_t velocity) {
    melodic_patch_factories[patch_id](voice, pitch, velocity);
}

void InstrumentRegistry::configure_drum_voice(std::uint8_t patch_id, Voice* voice, uint8_t pitch, uint8_t velocity) {
    drum_patch_factories[patch_id](voice, pitch, velocity);
}