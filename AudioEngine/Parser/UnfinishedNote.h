#ifndef MIDI_PARSERSYNTHESIZER_UNFINISHEDNOTE_H
#define MIDI_PARSERSYNTHESIZER_UNFINISHEDNOTE_H
#include <cstdint>

struct UnfinishedNote {
    uint32_t start_time;
    uint8_t velocity;
};

#endif //MIDI_PARSERSYNTHESIZER_UNFINISHEDNOTE_H