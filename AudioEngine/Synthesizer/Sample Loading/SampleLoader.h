#ifndef MIDI_PARSERSYNTHESIZER_SAMPLELOADER_H
#define MIDI_PARSERSYNTHESIZER_SAMPLELOADER_H
#include <string>
#include <vector>

#include "RawAudioData.h"

class SampleLoader {
public:
    SampleLoader();

    RawAudioData load_wav_mono(const std::string& file_path);
};

#endif //MIDI_PARSERSYNTHESIZER_SAMPLELOADER_H