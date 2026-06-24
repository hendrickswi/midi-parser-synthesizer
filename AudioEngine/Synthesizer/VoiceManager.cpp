#include "VoiceManager.h"
#include "Voices/Voice.h"

#include <cmath>
#include <iostream>

#include "../Patch Configuration/InstrumentRegistry.h"
#include "../../EventTypeEnums/ContinuousControllers.h"

VoiceManager::VoiceManager(float sample_rate, float global_volume)
    : command_queue() {
    voices = std::array<Voice*, NUM_VOICES>();
    for (auto& voice : voices) {
        voice = new Voice();
    }

    channel_patches = std::array<uint8_t, NUM_CHANNELS>();
    channel_patches.fill(0);

    registry = new InstrumentRegistry(sample_rate);

    channel_pitch_bends = std::array<uint16_t, NUM_CHANNELS>();
    channel_pitch_bends.fill(8192);

    channel_pressures = std::array<uint8_t, NUM_CHANNELS>();
    channel_pressures.fill(0);

    channel_cc_states = std::array<std::array<uint8_t, 128>, NUM_CHANNELS>();
    for (auto& channel_state : channel_cc_states) {
        channel_state.fill(0);
    }
    for (int i = 0; i < 16; i++) {
        channel_cc_states[i][7] = 100;  // Channel volume defaults to 100
        channel_cc_states[i][11] = 127; // Expression defaults to max
        channel_cc_states[i][10] = 64;  // Pan defaults to center
    }

    this->sample_rate = sample_rate;

    if (global_volume < 0.0f || global_volume > 2.0f) {
        std::cout << "WARNING: Invalid global volume value, defaulting to 1.0" << std::endl;
        global_volume = 1.0f;
    }
    this->global_volume = global_volume;

    static_gain = std::sqrt(static_cast<float>(NUM_VOICES)); // Somewhat safe default
    peak_amplitude_normalization_on = true;
}

VoiceManager::~VoiceManager() {
    for (auto& voice : voices) {
        delete voice;
    }

    delete registry;
}

bool VoiceManager::get_peak_amplitude_normalization() const {
    return peak_amplitude_normalization_on;
}

bool VoiceManager::all_voices_free() const {
    for (const auto& voice : voices) {
        if (!voice->is_free()) return false;
    }
    return true;
}

float VoiceManager::get_sample_rate() const {
    return sample_rate;
}

float VoiceManager::get_global_volume() const {
    return global_volume;
}

float VoiceManager::get_static_gain() const {
    return static_gain;
}

void VoiceManager::set_sample_rate(float sample_rate) {
    this->sample_rate = sample_rate;
    registry->set_sample_rate(sample_rate);
}

void VoiceManager::set_global_volume(float global_volume) {
    if (global_volume < 0.0f || global_volume > 2.0f) return;
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
    if (channel >= NUM_CHANNELS) return;
    channel_cc_states[channel][cc_number] = cc_value;

    // Push changes down to only active voices (for performance)
    for (auto& voice : voices) {
        if (!voice->is_free() && voice->get_channel() == channel) {
            voice->update_cc(cc_number, cc_value);
        }
    }
}

void VoiceManager::set_static_gain(float gain) {
    static_gain = gain;
}

void VoiceManager::set_peak_amplitude_normalization(bool enabled) {
    peak_amplitude_normalization_on = enabled;
}

void VoiceManager::load_patch_configs(const std::set<uint8_t>& melodic_patch_numbers, const std::set<uint8_t>& drum_patch_numbers) {
    registry->load_patches(melodic_patch_numbers, drum_patch_numbers);
}

void VoiceManager::unload_all_patch_configs() {
    registry->clear_patches();
}

void VoiceManager::note_on(uint8_t channel, uint8_t pitch, uint8_t velocity) {
    if (channel >= NUM_CHANNELS) return;
    if (velocity == 0) {
        note_off(channel, pitch);
        return;
    }

    // Fetch patch config from the registry
    uint8_t patch_id = channel_patches[channel];
    const PatchDefinition* patch_config = nullptr;
    if (channel == 9) {
        patch_config = registry->get_drum_patch_config(pitch);
    }
    else {
        patch_config = registry->get_melodic_patch_config(patch_id);
    }

    // Look for a free voice, but also track the oldest voices in case none are free
    int voice_idx = -1, oldest_released_voice_idx = 0, oldest_sustained_voice_idx = 0, oldest_active_voice_idx = 0;
    for (int i = 0; i < NUM_VOICES; i++) {
        if (voices[i]->is_free()) {
            voice_idx = i;
            break;
        }
        if (voices[i]->is_released() && voices[i]->get_note_activation_time() < voices[oldest_released_voice_idx]->get_note_activation_time()) {
            oldest_released_voice_idx = i;
        }
        if (voices[i]->is_released() && voices[i]->is_sustained() && voices[i]->get_note_activation_time() < voices[oldest_active_voice_idx]->get_note_activation_time()) {
            oldest_sustained_voice_idx = i;
        }
        if (voices[i]->get_note_activation_time() < voices[oldest_active_voice_idx]->get_note_activation_time()) {
            oldest_active_voice_idx = i;
        }
    }

    // Voice steal on the oldest start time if no free voice was found
    // Priority on voices in release phase, then sustained but not released, then oldest.
    if (voice_idx == -1) {
        if (oldest_released_voice_idx != -1) {
            voice_idx = oldest_released_voice_idx;
        }
        else if (oldest_sustained_voice_idx != -1) {
            voice_idx = oldest_sustained_voice_idx;
        }
        else {
            voice_idx = oldest_active_voice_idx;
        }

        voices[voice_idx]->note_off();
        // std::cout << "INFO: Voice " << voice_idx << " stolen" << std::endl;
    }

    // Pass the voice the entire cc states
    for (int i = 0; i < 128; i++) {
        voices[voice_idx]->update_cc(i, channel_cc_states[channel][i]);
    }

    voices[voice_idx]->configure_and_note_on(patch_config, channel, pitch, velocity, sample_rate);
}

void VoiceManager::note_off(uint8_t channel, uint8_t pitch) {
    if (channel >= NUM_CHANNELS) return;

    // Only turn off the oldest voice that is currently held, with matching channel and pitch.
    int target_voice = -1;
    for (int i = 0; i < NUM_VOICES; i++) {
        auto& voice = voices[i];
        if (voice->get_channel() == channel && voice->get_pitch() == pitch && voice->is_key_held()) {
            if (target_voice == -1 || voice->get_note_activation_time() < voices[target_voice]->get_note_activation_time()) {
                target_voice = i;
            }
        }
    }

    if (target_voice != -1) {
        voices[target_voice]->note_off();
    }
}

void VoiceManager::process_audio_buffer(float* buffer, const unsigned int num_samples) {
    std::fill(buffer, buffer + num_samples, 0.0f);

    // Process by block for performance
    for (const auto& voice : voices) {
        if (!voice->is_free()) {
            voice->process_block(buffer, num_samples);
        }
    }

    // Then apply volume edits
    for (int i = 0; i < num_samples; i++) {
        float mix = buffer[i] * global_volume;
        if (peak_amplitude_normalization_on) {
            mix *= static_gain;
        }


        const float sign = (mix > 0.0f) ? 1.0f : -1.0f;
        const float abs_mix = mix * sign;

        if (abs_mix > 1.0f) {
            buffer[i] = sign * 1.0f;
        }
        else {
            // [3/4] Pade approximation for std::tanh
            const float mix_squared = abs_mix * abs_mix;
            buffer[i] = sign * (abs_mix * (27.0f + mix_squared) / (27.0f + 9.0f * mix_squared));
        }
    }
}

void VoiceManager::stop() {
    for (auto& voice : voices) {
        voice->update_cc(SUSTAIN_PEDAL, 0);
        voice->note_off();
    }
}

void VoiceManager::reset_state() {
    stop();

    channel_pitch_bends.fill(8192);
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

bool VoiceManager::push_to_command_queue(const SynthesizerCommand& command) {
    return command_queue.push(command);
}

bool VoiceManager::pop_from_command_queue(SynthesizerCommand& command) {
    return command_queue.pop(command);
}

bool VoiceManager::peek_from_command_queue(SynthesizerCommand& command) {
    return command_queue.peek(command);
}

void VoiceManager::clear_command_queue() {
    SynthesizerCommand command;
    while (pop_from_command_queue(command)) {
    }
}
