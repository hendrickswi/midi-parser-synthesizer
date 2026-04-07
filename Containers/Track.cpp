#include "Track.h"

Track::Track() {
    notes = std::vector<Note>();
    midi_events = std::vector<MidiEvent>();
    sysex_events = std::vector<SysexEvent>();
    meta_events = std::vector<MetaEvent>();
}

Track::Track(const std::vector<Note>& notes, const std::vector<MidiEvent>& midi_events,
             const std::vector<SysexEvent>& sysex_events, const std::vector<MetaEvent>& meta_events) {
    this->notes = notes;
    this->midi_events = midi_events;
    this->sysex_events = sysex_events;
    this->meta_events = meta_events;
}

Track::Track(const Track& other) {
    notes = other.notes;
    midi_events = other.midi_events;
    sysex_events = other.sysex_events;
    meta_events = other.meta_events;
}

void Track::add_note(const Note& note) {
    notes.push_back(note);
}

void Track::add_midi_event(const MidiEvent& event) {
    midi_events.push_back(event);
}

void Track::add_sysex_event(const SysexEvent& event) {
    sysex_events.push_back(event);
}

void Track::add_meta_event(const MetaEvent& event) {
    meta_events.push_back(event);
}

[[nodiscard]] const std::vector<Note>& Track::get_notes() const {
    return notes;
}

[[nodiscard]] const std::vector<MidiEvent>& Track::get_midi_events() const {
    return midi_events;
}

[[nodiscard]] const std::vector<SysexEvent>& Track::get_sysex_events() const {
    return sysex_events;
}

[[nodiscard]] const std::vector<MetaEvent>& Track::get_meta_events() const {
    return meta_events;
}
