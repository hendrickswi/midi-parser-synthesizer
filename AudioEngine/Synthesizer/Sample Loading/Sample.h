#ifndef MIDI_PARSERSYNTHESIZER_SAMPLE_H
#define MIDI_PARSERSYNTHESIZER_SAMPLE_H
#include <cstdint>
#include <vector>

struct Sample {
    std::vector<float> audio_buffer;
    float base_frequency;
    uint8_t min_pitch;
    uint8_t max_pitch;
};

#endif //MIDI_PARSERSYNTHESIZER_SAMPLE_H