#ifndef MIDI_PARSERSYNTHESIZER_VOICEMANAGER_H
#define MIDI_PARSERSYNTHESIZER_VOICEMANAGER_H
#include <array>
#include <cstdint>

#include "LockFreeQueue.h"
#include "SynthesizerCommand.h"
#include "../Patch Configuration/InstrumentRegistry.h"
#include "../Synthesizer/Voices/Voice.h"

static constexpr unsigned int NUM_VOICES = 256;
static constexpr unsigned int NUM_CHANNELS = 16;

class VoiceManager {
private:
    std::array<Voice*, NUM_VOICES> voices;
    std::array<uint8_t, NUM_CHANNELS> channel_patches;
    InstrumentRegistry* registry;
    LockFreeQueue<SynthesizerCommand, 1024> command_queue;

    // Midi-event specific data
    std::array<uint16_t, NUM_CHANNELS> channel_pitch_bends;
    std::array<uint8_t, NUM_CHANNELS> channel_pressures;
    std::array<std::array<uint8_t, 128>, NUM_CHANNELS> channel_cc_states;

    float sample_rate;
    float global_volume;
    float static_gain;
    bool peak_amplitude_normalization_on;

public:
    explicit VoiceManager(float sample_rate = 48000.0f, float global_volume = 1.0f);
    VoiceManager(const VoiceManager& other) = delete;

    ~VoiceManager();

    VoiceManager& operator=(const VoiceManager& other) = delete;

    [[nodiscard]] bool get_peak_amplitude_normalization() const;

    /**
     * Sets the current sample rate.
     *
     * @remarks This method does *not* update currently loaded samples
     * See @c unload_all_patch_samples() and @c load_patch_samples() for
     * this functionality.
     *
     * @param sample_rate A float value representing the new sample rate.
     */
    void set_sample_rate(float sample_rate);

    void set_global_volume(float global_volume);
    void set_channel_patch(uint8_t channel, uint8_t program_number);
    void set_channel_pitch_bend(uint8_t channel, uint16_t pitch_bend);
    void set_channel_pressure(uint8_t channel, uint8_t pressure);
    void set_channel_cc(uint8_t channel, uint8_t cc_number, uint8_t cc_value);
    void set_static_gain(float gain);
    void set_peak_amplitude_normalization(bool enabled);

    /**
     * Loads the requested patch configurations, represented by vectors of patch ids,
     * into memory with the current sample rate.
     */
    void load_patch_configs(const std::set<uint8_t>& melodic_patch_numbers, const std::set<uint8_t>& drum_patch_numbers);

    /**
     * Unloads all the loaded patch configurations, clearing memory.
     */
    void unload_all_patch_configs();

    void note_on(uint8_t channel, uint8_t pitch, uint8_t velocity);
    void note_off(uint8_t channel, uint8_t pitch);
    void process_audio_buffer(float* buffer, unsigned int num_samples);
    void stop();
    void reset_state();

    // Sequencer/AudioEngine::audio_callback() controls
    bool push_to_command_queue(const SynthesizerCommand& command);
    bool pop_from_command_queue(SynthesizerCommand& command);
    bool peek_from_command_queue(SynthesizerCommand& command);
    void clear_command_queue();
};


#endif //MIDI_PARSERSYNTHESIZER_VOICEMANAGER_H