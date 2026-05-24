#include "VoiceManager.h"
#include "Voices/Voice.h"

#include <cmath>

#include "../Patch Configuration/InstrumentRegistry.h"
#include "Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
#include "../../EventTypeEnums/ContinuousControllers.h"
#include "Oscillators/Base Implementations/Algorithmic/Sine/SineOscillator.h"

void VoiceManager::init(float sample_rate, float global_volume) {
    this->sample_rate = sample_rate;
    this->global_volume = global_volume;
    headroom_attenuation = std::sqrt(static_cast<float>(NUM_VOICES));

    voices = std::array<std::unique_ptr<Voice>, NUM_VOICES>();
    channel_patches = std::array<uint8_t, NUM_CHANNELS>();
    registry = std::make_unique<InstrumentRegistry>(sample_rate);

    // Midi event specific data
    channel_pitch_bends = std::array<uint16_t, NUM_CHANNELS>();
    channel_pressures = std::array<uint8_t, NUM_CHANNELS>();
    channel_cc_states = std::array<std::array<uint8_t, 128>, NUM_CHANNELS>();

    // Creating all the voices
    for (auto& voice : voices) {
        auto oscillator = std::make_unique<SineOscillator>();
        auto envelope = std::make_unique<ADSREnvelope>();
        voice = std::make_unique<Voice>(std::move(oscillator), std::move(envelope));
    }

    channel_patches.fill(0);

    for (auto& channel_state : channel_cc_states) {
        channel_state.fill(0);
    }

    for (int i = 0; i < 16; i++) {
        channel_cc_states[i][7] = 100;  // Channel volume defaults to 100
        channel_cc_states[i][11] = 127; // Expression defaults to max
        channel_cc_states[i][10] = 64;  // Pan defaults to center
    }
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
    if (channel >= NUM_CHANNELS) return;
    channel_patches[channel] = program_number;
}

void VoiceManager::set_channel_pitch_bend(const uint8_t channel, const uint16_t pitch_bend) {
    if (channel >= NUM_CHANNELS) return;
    channel_pitch_bends[channel] = pitch_bend;
}

void VoiceManager::set_channel_pressure(const uint8_t channel, const uint8_t pressure) {
    if (channel >= NUM_CHANNELS) return;
    channel_pressures[channel] = pressure;
}

void VoiceManager::set_channel_cc(uint8_t channel, uint8_t cc_number, uint8_t cc_value) {
    std::lock_guard<std::mutex> lock(audio_mutex);

    if (channel >= NUM_CHANNELS) return;
    channel_cc_states[channel][cc_number] = cc_value;

    // Push changes down to only active voices (for performance)
    for (auto& voice : voices) {
        if (!voice->is_free() && voice->get_channel() == channel) {
            voice->update_cc(cc_number, cc_value);
        }
    }
}

void VoiceManager::note_on(uint8_t channel, uint8_t pitch, uint8_t velocity) {
    std::lock_guard<std::mutex> lock(audio_mutex);

    if (channel >= NUM_CHANNELS) return;
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

    // Pass the voice the entire cc states
    for (int i = 0; i < 128; i++) {
        voices[voice_idx]->update_cc(i, channel_cc_states[channel][i]);
    }

    // Ensure the voice has the correct oscillator/envelope
    if (channel == 9) {
        // Index by pitch
        registry->configure_drum_voice(pitch, voices[voice_idx].get(), pitch, velocity);
    }
    else {
        // Index by patch id
        uint8_t patch_id = channel_patches[channel];
        registry->configure_melodic_voice(patch_id, voices[voice_idx].get(), pitch, velocity);
    }

    voices[voice_idx]->note_on(channel, pitch, velocity, sample_rate);
}

void VoiceManager::note_off(uint8_t channel, uint8_t pitch) {
    std::lock_guard<std::mutex> lock(audio_mutex);

    for (auto& voice : voices) {
        if (voice->get_channel() == channel && voice->get_pitch() == pitch) {
            voice->note_off();
        }
    }
}

void VoiceManager::process_audio_buffer(float* buffer, const unsigned int num_samples) {
    std::lock_guard<std::mutex> lock(audio_mutex);

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

        float safe_mix = instruction / headroom_attenuation;
        buffer[i] = std::tanh(safe_mix * global_volume);
    }
}

void VoiceManager::stop() {
    for (auto& voice : voices) {
        // Force sustain pedal up
        voice->update_cc(SUSTAIN_PEDAL, 0);

        // Then turn off the note
        voice->note_off();
    }
}

void VoiceManager::reset_state() {
    std::lock_guard<std::mutex> lock(audio_mutex);

    stop();
    channel_pitch_bends.fill(0);
    channel_pressures.fill(0);
    for (auto& channel_state : channel_cc_states) {
        channel_state.fill(0);
    }
    for (int i = 0; i < 16; i++) {
        channel_cc_states[i][7] = 100;  // Channel volume defaults to 100
        channel_cc_states[i][11] = 127; // Expression defaults to max
        channel_cc_states[i][10] = 64;  // Pan defaults to center
    }
}
