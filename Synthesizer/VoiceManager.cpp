#include "VoiceManager.h"
#include "Voices/Voice.h"

#include <cmath>

#include "Oscillators/Base Implementations/SineOscillator.h"
#include "Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
#include "Oscillators/Base Implementations/SawtoothOscillator.h"
#include "Oscillators/Base Implementations/SquareOscillator.h"

void VoiceManager::init(float sample_rate, float global_volume) {
    this->sample_rate = sample_rate;
    this->global_volume = global_volume;

    voices = std::array<std::unique_ptr<Voice>, NUM_VOICES>();
    channel_patches = std::array<uint8_t, NUM_VOICES>();
    oscillator_factories = std::array<std::function<std::unique_ptr<Oscillator>()>, 128>();

    // Creating all the voices
    for (auto& voice : voices) {
        auto oscillator = std::make_unique<SineOscillator>();
        auto envelope = std::make_unique<ADSREnvelope>();
        voice = std::make_unique<Voice>(std::move(oscillator), std::move(envelope));
    }

    // Creating all the factories
    oscillator_factories[0] = []() { return std::make_unique<SineOscillator>(); };
    oscillator_factories[81] = []() { return std::make_unique<SquareOscillator>(); };
    oscillator_factories[82] = []() { return std::make_unique<SawtoothOscillator>(); };
    // TODO: Flesh out this list more

    channel_patches.fill(0);
}

VoiceManager::VoiceManager() { // NOLINT
    init();
}

VoiceManager::VoiceManager(float sample_rate, float global_volume) { // NOLINT
    if (global_volume < 0.0f || global_volume > 1.0f) {
        init(sample_rate);
        return;
    }

    init(sample_rate, global_volume);
}

VoiceManager::~VoiceManager() = default;

void VoiceManager::set_sample_rate(float sample_rate) {
    this->sample_rate = sample_rate;
}

void VoiceManager::set_global_volume(float global_volume) {
    this->global_volume = global_volume;
}

void VoiceManager::set_channel_patch(const uint8_t channel, const uint8_t program_number) {
    if (channel >= NUM_VOICES) return;
    channel_patches[channel] = program_number;
}

void VoiceManager::note_on(uint8_t channel, uint8_t pitch, uint8_t velocity) {
    if (channel >= NUM_VOICES) return;
    if (velocity == 0) {
        note_off(channel, pitch);
        return;
    }

    // Look for a free voice
    int voice_idx = -1, oldest_voice_idx = 0;
    for (int i = 0; i < NUM_VOICES; i++) {
        if (voices[i]->is_free()) {
            voice_idx = i;
            break;
        }

        if (voices[i]->get_note_activation_time() < voices[oldest_voice_idx]->get_note_activation_time()) {
            oldest_voice_idx = i;
        }
    }

    // Voice steal on the oldest start time if no free voice was found
    if (voice_idx == -1) {
        voice_idx = oldest_voice_idx;
        voices[voice_idx]->note_off();
    }

    uint8_t patch_id = channel_patches[channel];
    auto oscillator = oscillator_factories[patch_id]();
    voices[voice_idx]->set_oscillator(std::move(oscillator));
    voices[voice_idx]->note_on(channel, pitch, velocity, sample_rate);
}

void VoiceManager::note_off(uint8_t channel, uint8_t pitch) {
    for (auto& voice : voices) {
        if (voice->get_channel() == channel && voice->get_pitch() == pitch) {
            voice->note_off();
            return;
        }
    }
}

void VoiceManager::process_audio_buffer(float* buffer, const unsigned int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        buffer[i] = 0.0f;
    }

    for (int i = 0; i < num_samples; i++) {
        float instruction = 0.0f;
        for (const auto& voice : voices) {
            if (!voice->is_free()) {
                instruction += voice->process();
            }
        }
        buffer[i] = std::tanh(instruction * global_volume);
    }
}

void VoiceManager::stop() {
    for (auto& voice : voices) {
        voice->note_off();
    }
}
