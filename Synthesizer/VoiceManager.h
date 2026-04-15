#ifndef MIDI_PARSERSYNTHESIZER_VOICEMANAGER_H
#define MIDI_PARSERSYNTHESIZER_VOICEMANAGER_H
#include <array>
#include "Voices/Voice.h"


class VoiceManager {
private:
    static constexpr int NUM_VOICES = 16;
    std::array<Voice, NUM_VOICES> voices;

    float sample_rate;
    float global_volume;

    [[nodiscard]] float pitch_to_hz(float pitch);

public:
    VoiceManager();
    VoiceManager(float sample_rate, float global_volume);
    VoiceManager(const VoiceManager& other);

    void set_sample_rate(float sample_rate);
    void set_global_volume(float global_volume);

    void note_on(float pitch, float velocity);
    void note_off(float pitch);
    void update(float* buffer, int num_samples);
};


#endif //MIDI_PARSERSYNTHESIZER_VOICEMANAGER_H