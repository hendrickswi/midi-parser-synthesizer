#ifndef MIDI_PARSERSYNTHESIZER_TRACK_H
#define MIDI_PARSERSYNTHESIZER_TRACK_H
#include <cstdint>
#include <vector>

#include "MidiEvent.h"
#include "Note.h"

/**
 * @class Track
 * @brief Represents a musical track containing a collection of @code note@endcode s and
 * @code MidiEvent@endcode s.
 *
 * The Track class is designed to store and manage musical information, specifically
 * a sequence of notes and any associated MIDI events. Each track can be treated
 * as a standalone unit in a musical composition.
 */
class Track {
private:
    std::vector<Note> notes;
    std::vector<MidiEvent> midi_events;

public:
    Track();
    Track(const std::vector<Note>& notes, const std::vector<MidiEvent>& midi_events);
    Track(const Track& other);

    void add_note(Note note);
    void add_midi_event(MidiEvent event);

    [[nodiscard]] const std::vector<Note>& get_notes() const { return notes; }
    [[nodiscard]] const std::vector<MidiEvent>& get_midi_events() const { return midi_events; }
};

#endif //MIDI_PARSERSYNTHESIZER_TRACK_H