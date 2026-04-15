#include "MidiSequencer.h"
#include "../EventTypeEnums/MetaEventType.h"

void MidiSequencer::init() {
    current_tick = 0;
    prev_tick_time = std::chrono::high_resolution_clock::now();
    micros_since_last_tick = 0;
    track_indices.clear();
    track_indices.resize(track_sequence.get_tracks().size());
}

[[nodiscard]] inline uint32_t MidiSequencer::calculate_micros_per_tick(const uint32_t& tempo) const {
    return 60000000 / tempo / track_sequence.get_division();
}

void MidiSequencer::process_events(const Track& track, TrackIndices& indices) {
    // Note processing
    const auto& notes = track.get_notes();
    while (indices.note_idx < notes.size()
        && notes[indices.note_idx].absolute_time <= current_tick) {

        const auto& note = notes[indices.note_idx];
        indices.note_idx++;

        // TODO: Activate note in synthesizer

        active_notes.emplace(note);
    }

    // Midi event processing
    const auto& midi_events = track.get_midi_events();
    while (indices.midi_event_idx < midi_events.size()
        && midi_events[indices.midi_event_idx].absolute_time <= current_tick) {

        const auto& midi_event = midi_events[indices.midi_event_idx];
        indices.midi_event_idx++;

        // TODO: Implement midi event logic
    }

    // Meta event processing
    const auto& meta_events = track.get_meta_events();
    while (indices.meta_idx < meta_events.size()
        && meta_events[indices.meta_idx].absolute_time <= current_tick) {

        const auto& meta_event = meta_events[indices.meta_idx];
        indices.meta_idx++;

        if (meta_event.type == TEMPO_SETTING) {
            // TODO: Implement tempo change method
        }

        // TODO: Implement any other relevant meta event logic
    }

    // Sysex event processing
    const auto& sysex_events = track.get_sysex_events();
    while (indices.sysex_idx < meta_events.size()
        && sysex_events[indices.sysex_idx].absolute_time <= current_tick) {

        const auto& sysex_event = sysex_events[indices.sysex_idx];
        indices.sysex_idx++;

        // TODO: Implement sysex event logic
    }
}

MidiSequencer::MidiSequencer() {
    track_sequence = TrackSequence();
    is_playing = false;

    micros_per_tick = calculate_micros_per_tick(120);
    init();
}

MidiSequencer::MidiSequencer(const TrackSequence& track_sequence) {
    this->track_sequence = track_sequence;
    is_playing = false;

    micros_per_tick = calculate_micros_per_tick(120);
    init();
}

MidiSequencer::MidiSequencer(const MidiSequencer& other) {
    track_sequence = other.track_sequence;
    is_playing = other.is_playing;

    micros_per_tick = other.micros_per_tick;
    current_tick = other.current_tick;
    prev_tick_time = other.prev_tick_time;
    micros_since_last_tick = other.micros_since_last_tick;
    track_indices = other.track_indices;
}

void MidiSequencer::start() {
    is_playing = true;
}

void MidiSequencer::stop() {
    is_playing = false;
}

void MidiSequencer::update() {
    if (!is_playing) return;
    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(current_time - prev_tick_time);
    micros_since_last_tick += elapsed_time.count();

    // Remove expired notes
    while (!active_notes.empty() && active_notes.top().end_time >= current_tick) {
        // TODO: Deactivate note in synthesizer
        active_notes.pop();
    }


    // Add new notes
    while (micros_since_last_tick >= micros_per_tick) {
        const auto& tracks = track_sequence.get_tracks();
        // Trigger events registered to the current tick
        micros_since_last_tick -= micros_per_tick;
        current_tick++;

        // Go through each "track" (each instrument/part)
        for (int i = 0; i < tracks.size(); i++) {
            const auto& track = tracks[i];
            auto& indices = track_indices[i];
            process_events(track, indices);
        }
    }
}

void MidiSequencer::reset() {
    is_playing = false;
    init();
}

const TrackSequence& MidiSequencer::get_track_sequence() const {
    return track_sequence;
}

void MidiSequencer::set_track_sequence(const TrackSequence& track_sequence) {
    this->track_sequence = track_sequence;
}

bool MidiSequencer::get_is_playing() const {
    return is_playing;
}