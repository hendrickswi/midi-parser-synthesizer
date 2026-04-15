
#ifndef MIDI_PARSERSYNTHESIZER_ACTIVENOTE_H
#define MIDI_PARSERSYNTHESIZER_ACTIVENOTE_H
#include <cstdint>
#include "../TrackEvents/Note.h"

struct ActiveNote {
    std::uint32_t end_time;
    std::uint8_t pitch;
    std::uint8_t channel;

    ActiveNote() = default;

    ActiveNote(const Note& note) {
        end_time = note.absolute_time + note.duration;
        pitch = note.pitch;
        channel = note.channel;
    }

    // The note with the sooner starting time should be "greater"
    // (for priority queue sorting).
    bool operator>(const ActiveNote& other) const {
        return end_time > other.end_time;
    }
};

#endif //MIDI_PARSERSYNTHESIZER_ACTIVENOTE_H