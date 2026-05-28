#ifndef MIDI_PARSERSYNTHESIZER_SAMPLE_H
#define MIDI_PARSERSYNTHESIZER_SAMPLE_H
#include <cstdint>
#include <vector>

struct Sample {
    std::vector<float> audio_buffer;
    float sample_rate;
    float base_frequency;
    uint8_t min_pitch;
    uint8_t max_pitch;
    uint8_t min_velocity;
    uint8_t max_velocity;

    Sample(std::vector<float> audio_buffer, float sample_rate, float base_frequency, uint8_t min_pitch, uint8_t max_pitch, uint8_t min_velocity = 0, uint8_t max_velocity = 127)
        : audio_buffer(audio_buffer), sample_rate(sample_rate), base_frequency(base_frequency), min_pitch(min_pitch), max_pitch(max_pitch), min_velocity(min_velocity), max_velocity(max_velocity) {
    }
};

#endif //MIDI_PARSERSYNTHESIZER_SAMPLE_H