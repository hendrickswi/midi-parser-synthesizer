#ifndef MIDI_PARSERSYNTHESIZER_VOICEMANAGER_H
#define MIDI_PARSERSYNTHESIZER_VOICEMANAGER_H
#include <array>
#include <cstdint>
#include <functional>
#include <memory>

// pimpl forward declaration
class Voice;
class Oscillator;

class VoiceManager {
private:
    static constexpr int NUM_VOICES = 16;
    std::array<std::unique_ptr<Voice>, NUM_VOICES> voices;
    std::array<uint8_t, NUM_VOICES> channel_patches;
    std::array<std::function<std::unique_ptr<Oscillator>()>, 128> oscillator_factories;

    float sample_rate;
    float global_volume;
    float headroom_attenuation;

    void init(float sample_rate = 44100.0f, float global_volume = 0.8f);

public:
    VoiceManager();
    VoiceManager(float sample_rate, float global_volume);
    VoiceManager(const VoiceManager& other) = delete;

    ~VoiceManager();

    VoiceManager& operator=(const VoiceManager& other) = delete;

    void set_sample_rate(float sample_rate);
    void set_global_volume(float global_volume);
    void set_channel_patch(uint8_t channel, uint8_t program_number);

    void note_on(uint8_t channel, uint8_t pitch, uint8_t velocity);
    void note_off(uint8_t channel, uint8_t pitch);
    void process_audio_buffer(float* buffer, unsigned int num_samples);
    void stop();
};


#endif //MIDI_PARSERSYNTHESIZER_VOICEMANAGER_H