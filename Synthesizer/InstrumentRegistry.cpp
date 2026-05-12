#include "InstrumentRegistry.h"

#include <memory>

#include "Envelopes/Base Implementations/ADR/ADREnvelope.h"
#include "Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
#include "Oscillators/Base Implementations/NoiseOscillator.h"
#include "Oscillators/Base Implementations/SawtoothOscillator.h"
#include "Oscillators/Base Implementations/SineOscillator.h"
#include "Oscillators/Base Implementations/SquareOscillator.h"
#include "Oscillators/Base Implementations/TriangleOscillator.h"
#include "Voices/Voice.h"

void InstrumentRegistry::init(float sample_rate) {
    this->sample_rate = sample_rate;
    init_pianos();
    init_organs();
    init_leads();
    init_drums();

    for (auto& factory : patch_factories) {
        if (factory == nullptr) {
            // Fallback
            factory = [sample_rate](Voice* v) {
                v->set_oscillator(std::make_unique<SineOscillator>(440.0f, sample_rate));
                v->set_envelope(std::make_unique<ADSREnvelope>());
            };
        }
    }

    for (auto& factory : drum_patch_factories) {
        if (factory == nullptr) {
            // Fallback
            factory = [sample_rate](Voice* v) {
                v->set_oscillator(std::make_unique<NoiseOscillator>());
                v->set_envelope(std::make_unique<ADREnvelope>(sample_rate, 0.005f, 1.0f,
                    0.1f, 0.1f, 0.1f, 0.0f));
            };
        }
    }
}

void InstrumentRegistry::init_pianos() {
    for (int i = 0; i < 8; i++) {
        patch_factories[i] = [sample_rate = this->sample_rate](Voice* v) {
            v->set_oscillator(std::make_unique<SineOscillator>(440.0f, sample_rate));

            // Sustain level at 0.05f and decay time at 3.0f to emulate a real piano
            v->set_envelope(std::make_unique<ADSREnvelope>(sample_rate, 0.005f, 1.0f,
                3.0f, 0.025f, 0.2f, 0.0f));
        };
    }
}

void InstrumentRegistry::init_organs() {
    for (int i = 16; i < 24; i++) {
        patch_factories[i] = [sample_rate = this->sample_rate](Voice* v) {
            v->set_oscillator(std::make_unique<SineOscillator>(440.0f, sample_rate));

            // Sustain level nearly full
            v->set_envelope(std::make_unique<ADSREnvelope>(sample_rate, 0.01f, 1.0f,
                0.01f, 0.95f, 0.05f, 0.0f));
        };
    }
}

void InstrumentRegistry::init_leads() {
    patch_factories[80] = [sample_rate = this->sample_rate](Voice* v) {
        v->set_oscillator(std::make_unique<SquareOscillator>(440.0f, sample_rate));
        v->set_envelope(std::make_unique<ADSREnvelope>(sample_rate, 0.005f, 1.0f, 0.2f,
            0.25f, 0.1f, 0.0f));
    };
    patch_factories[81] = [sample_rate = this->sample_rate](Voice* v) {
        v->set_oscillator(std::make_unique<SawtoothOscillator>(440.0f, sample_rate));
        v->set_envelope(std::make_unique<ADSREnvelope>(sample_rate, 0.005f, 1.0f, 0.2f,
            0.25f, 0.1f, 0.0f));
    };
    patch_factories[82] = [sample_rate = this->sample_rate](Voice* v) {
        v->set_oscillator(std::make_unique<TriangleOscillator>(440.0f, sample_rate));
        v->set_envelope(std::make_unique<ADSREnvelope>(sample_rate, 0.005f, 1.0f, 0.2f,
            0.25f, 0.1f, 0.0f));
    };
    // Can add more leads here later (e.g., calliope lead)
}

void InstrumentRegistry::init_drums() {
    // Kick drum
    drum_patch_factories[35] = [sample_rate = this->sample_rate](Voice* v) {
        v->set_oscillator(std::make_unique<SineOscillator>(55.0f, sample_rate));
        v->set_envelope(std::make_unique<ADREnvelope>(sample_rate, 0.005f, 1.0f, 0.1f,
             0.1f, 0.1f, 0.0f));
    };
    drum_patch_factories[36] = drum_patch_factories[35];

    // Snare drum
    drum_patch_factories[38] = [sample_rate = this->sample_rate](Voice* v) {
        v->set_oscillator(std::make_unique<NoiseOscillator>());
        v->set_envelope(std::make_unique<ADREnvelope>(sample_rate, 0.005f, 1.0f, 0.2f,
            0.25f, 0.1f, 0.0f));
    };
    drum_patch_factories[40] = drum_patch_factories[38];

    // Closed hi-hat
    drum_patch_factories[42] = [sample_rate = this->sample_rate](Voice* v) {
        v->set_oscillator(std::make_unique<NoiseOscillator>());
        v->set_envelope(std::make_unique<ADREnvelope>(sample_rate, 0.002f, 0.7f,
            0.05f, 0.05f, 0.0f, 0.0f));
    };

    // Open hi-hat
    drum_patch_factories[46] = [sample_rate = this->sample_rate](Voice* v) {
        v->set_oscillator(std::make_unique<NoiseOscillator>());
        v->set_envelope(std::make_unique<ADREnvelope>(sample_rate, 0.005f, 0.7f,
            0.4f, 0.1f, 0.0f, 0.0f));
    };
}

InstrumentRegistry::InstrumentRegistry() { // NOLINT
    init();
}

InstrumentRegistry::InstrumentRegistry(float sample_rate) { // NOLINT
    init(sample_rate);
}

void InstrumentRegistry::configure_melodic_voice(std::uint8_t patch_id, Voice* voice) {
    patch_factories[patch_id](voice);
}

void InstrumentRegistry::configure_drum_voice(std::uint8_t patch_id, Voice* voice) {
    drum_patch_factories[patch_id](voice);
}