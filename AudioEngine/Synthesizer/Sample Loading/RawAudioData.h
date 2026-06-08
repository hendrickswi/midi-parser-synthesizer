#ifndef MIDI_PARSERSYNTHESIZER_RAWAUDIODATA_H
#define MIDI_PARSERSYNTHESIZER_RAWAUDIODATA_H
#include <vector>

struct RawAudioData {
    std::vector<float> audio_buffer;
    float sample_rate;
};

#endif //MIDI_PARSERSYNTHESIZER_RAWAUDIODATA_H