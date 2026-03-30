#ifndef MIDI_PARSERSYNTHESIZER_TRACK_H
#define MIDI_PARSERSYNTHESIZER_TRACK_H

#include <cstdint>
#include <vector>
#include "TrackEvents/MidiEvent.h"
#include "TrackEvents/Note.h"
#include "TrackEvents/SysexEvent.h"
#include "TrackEvents/MetaEvent.h"

struct SysexEvent;
/**
 * @class Track
 * @brief Represents a musical track containing a collection of @code Note@endcode  and
 * @code MidiEvent@endcode.
 *
 * The Track class is designed to store and manage musical information, specifically
 * a sequence of notes and any associated MIDI events. Each track can be treated
 * as a standalone unit in a musical composition.
 */
class Track {
private:
    std::vector<Note> notes;
    std::vector<MidiEvent> midi_events;
    std::vector<SysexEvent> sysex_events;
    std::vector<MetaEvent> meta_events;

public:
    Track();
    Track(const std::vector<Note>& notes, const std::vector<MidiEvent>& midi_events,
        const std::vector<SysexEvent>& sysex_event, const std::vector<MetaEvent>& meta_events);
    Track(const Track& other);

    void add_note(Note note);
    void add_midi_event(MidiEvent event);
    void add_sysex_event(SysexEvent event);
    void add_meta_event(MetaEvent event);

    [[nodiscard]] const std::vector<Note>& get_notes() const { return notes; }
    [[nodiscard]] const std::vector<MidiEvent>& get_midi_events() const { return midi_events; }
};

#endif //MIDI_PARSERSYNTHESIZER_TRACK_H