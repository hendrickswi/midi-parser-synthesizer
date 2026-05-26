#include "InstrumentRegistry.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "PatchFactory.h"
#include "../Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelope.h"
#include "../Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
#include "../Synthesizer/Oscillators/Base Implementations/Noise/NoiseOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/Sample/SampleOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/Algorithmic/Sawtooth/SawtoothOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/Algorithmic/Sine/SineOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/Algorithmic/Square/SquareOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/Algorithmic/Triangle/TriangleOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/Composite/CompositeOscillator.h"
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
            /*
             * Sample selection logic:
             *      1st pass: Try to find a sample with matching velocity
             *      Else: Use the first loaded sample
             */

            const auto& samples = this->drum_patch_configs[drum_key].samples;
            const Sample* selected_sample = nullptr;

            for (const auto& sample : samples) {
                if (velocity >= sample.min_velocity && velocity <= sample.max_velocity) {
                    selected_sample = &sample;
                    break;
                }
            }

            if (selected_sample == nullptr) {
                selected_sample = &samples.front();
            }

            // Force 1.0x playback for drums
            float target_freq = 440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f);

            v->set_oscillator(PatchFactory::create_sample_oscillator(
                &selected_sample->audio_buffer,
                target_freq
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
                v->set_envelope(PatchFactory::create_adsr_envelope(this->sample_rate, 0.001f, 0.8f, 0.1f,
                0.6f, 0.03f, 0.0f));
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
            uint8_t max_velocity = zone.value("max_velocity", 127);

            std::vector<float> audio_data = loader.load_wav_mono(file_path);
            if (audio_data.empty()) {
                std::cerr << "Drum data buffer is completely empty for file: " << file_path << std::endl;
            }
            drum_patch_configs[drum_key].samples.push_back(Sample(
                audio_data, base_freq, 0, 127, min_velocity, max_velocity)
            );
        }
    }
}

void InstrumentRegistry::init_leads() {
    // Square lead
    melodic_patch_factories[80] = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
        v->set_oscillator(PatchFactory::create_square_oscillator(440.0f, sample_rate));
        v->set_envelope(PatchFactory::create_adsr_envelope(sample_rate, 0.005f, 0.6f, 0.2f,
            0.25f, 0.1f, 0.0f));
    };

    // Sawtooth lead
    melodic_patch_factories[81] = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
        v->set_oscillator(PatchFactory::create_sawtooth_oscillator(440.0f, sample_rate));
        v->set_envelope(PatchFactory::create_adsr_envelope(sample_rate, 0.005f, 0.6f, 0.2f,
            0.25f, 0.1f, 0.0f));
    };

    // Calliope lead
    melodic_patch_factories[82] = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
        v->set_oscillator(PatchFactory::create_triangle_oscillator(440, sample_rate));
        v->set_envelope(PatchFactory::create_adsr_envelope(sample_rate, 0.02f, 0.6f, 0.1f, 0.7f, 0.2f, 0.0f));
    };

    // Chiff lead
    melodic_patch_factories[83] = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
        v->set_oscillator(PatchFactory::create_sine_oscillator(440.0f, sample_rate));
        v->set_envelope(PatchFactory::create_adsr_envelope(sample_rate, 0.005f, 0.6f, 0.15f, 0.4f, 0.15f, 0.0f));
    };

    // Charang lead
    melodic_patch_factories[84] = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
        v->set_oscillator(PatchFactory::create_square_oscillator(440.0f, sample_rate));
        v->set_envelope(PatchFactory::create_adsr_envelope(sample_rate, 0.005f, 0.6f, 0.3f, 0.2f, 0.15f, 0.0f));
    };

    // Voice lead
    melodic_patch_factories[85] = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
        v->set_oscillator(PatchFactory::create_triangle_oscillator(440.0f, sample_rate));
        v->set_envelope(PatchFactory::create_adsr_envelope(sample_rate, 0.06f, 0.6f, 0.1f, 0.7f, 0.25f, 0.0f));
    };

    // Fifths lead
    melodic_patch_factories[86] = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
        auto composite = std::make_unique<CompositeOscillator>();
        composite->add_oscillator(PatchFactory::create_sawtooth_oscillator(440.0f, sample_rate), 0.6f, 1.0f);
        composite->add_oscillator(PatchFactory::create_sawtooth_oscillator(440.0f, sample_rate), 0.4f, std::pow(2.0f, 7.0f / 12.0f));
        v->set_oscillator(std::move(composite));
        v->set_envelope(PatchFactory::create_adsr_envelope(sample_rate, 0.005f, 0.6f, 0.15f, 0.4f, 0.15f, 0.0f));
    };

    // Bass + lead
    melodic_patch_factories[87] = [this](Voice* v, uint8_t pitch, uint8_t velocity) {
        auto composite = std::make_unique<CompositeOscillator>();
        composite->add_oscillator(PatchFactory::create_sawtooth_oscillator(440.0f, sample_rate), 0.45f, 1.0f);
        composite->add_oscillator(PatchFactory::create_square_oscillator(440.0f, sample_rate), 0.55f, 0.5f);

        v->set_oscillator(std::move(composite));
        v->set_envelope(PatchFactory::create_adsr_envelope(sample_rate, 0.005f, 0.8f, 0.2f, 0.5f, 0.1f, 0.0f));
    };
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
    drum_patch_factories[pitch](voice, pitch, velocity);
}