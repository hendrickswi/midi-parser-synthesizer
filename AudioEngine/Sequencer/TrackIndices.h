#ifndef MIDI_PARSERSYNTHESIZER_TRACKINDICES_H
#define MIDI_PARSERSYNTHESIZER_TRACKINDICES_H
#include <cstddef>

struct TrackIndices {
    std::size_t note_idx;
    std::size_t midi_event_idx;
    std::size_t sysex_idx;
    std::size_t meta_idx;
};

#endif //MIDI_PARSERSYNTHESIZER_TRACKINDICES_H