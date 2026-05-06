#include "MidiSequencer.h"
#include "../EventTypeEnums/MetaEventType.h"
#include "../EventTypeEnums/MidiEventType.h"


[[nodiscard]] inline uint32_t calculate_mpqn(const uint32_t tempo) {
    return 60000000 / tempo;
}

[[nodiscard]] inline uint32_t calculate_mpt(const uint32_t mpqn, const uint16_t division) {
    return mpqn / division;
}

MidiSequencer::MidiSequencer() { // NOLINT
    track_sequence = TrackSequence();
    synthesizer = nullptr;
    init();
}

MidiSequencer::MidiSequencer(const TrackSequence& track_sequence, VoiceManager* synthesizer) { // NOLINT
    this->track_sequence = track_sequence;
    this->synthesizer = synthesizer;
    init();
}

MidiSequencer::MidiSequencer(const MidiSequencer& other) {
    track_sequence = other.track_sequence;
    synthesizer = other.synthesizer;
    is_playing_flag = other.is_playing_flag;
    midi_file_ended_flag = other.midi_file_ended_flag;
    micros_per_tick = other.micros_per_tick;
    current_tick = other.current_tick;
    prev_tick_time = other.prev_tick_time;
    micros_since_last_tick = other.micros_since_last_tick;
    track_indices = other.track_indices;
}

void MidiSequencer::init() {
    is_playing_flag = false;
    midi_file_ended_flag = false;
    current_tick = 0;
    prev_tick_time = std::chrono::high_resolution_clock::now();
    micros_since_last_tick = 0;
    micros_per_tick = calculate_mpt(calculate_mpqn(120), track_sequence.get_division());
    track_indices.clear();

    // This also does default instantiation of TrackIndices structs
    track_indices.resize(track_sequence.get_tracks().size());
}

void MidiSequencer::process_events(const Track& track, TrackIndices& indices) {
    // Note processing
    const auto& notes = track.get_notes();
    while (indices.note_idx < notes.size()
        && notes[indices.note_idx].absolute_time <= current_tick) {

        const auto& note = notes[indices.note_idx];
        indices.note_idx++;
        synthesizer->note_on(note.channel, note.pitch, note.volume);
        active_notes.emplace(note);
    }

    // Midi event processing
    const auto& midi_events = track.get_midi_events();
    while (indices.midi_event_idx < midi_events.size()
        && midi_events[indices.midi_event_idx].absolute_time <= current_tick) {

        const auto& midi_event = midi_events[indices.midi_event_idx];
        indices.midi_event_idx++;

        uint8_t command = midi_event.status & 0xF0;
        uint8_t channel = midi_event.status & 0x0F;
        switch (command) {
            case PROGRAM_CHANGE: {
                synthesizer->set_channel_patch(channel, midi_event.data1);
                break;
            }
            case CONTROL_CHANGE : {
                synthesizer->set_channel_cc(channel, midi_event.data1, midi_event.data2);
                break;
            }

            case PITCH_BEND : {
                synthesizer->set_channel_pitch_bend(channel, (midi_event.data1 & 0x7F) | ((midi_event.data2 & 0x7F ) << 7));
                break;
            }

            case CHANNEL_PRESSURE : {
                synthesizer->set_channel_pressure(channel, midi_event.data1);
                break;
            }

            default : {
                break;
            }
        }


    }

    // Meta event processing
    const auto& meta_events = track.get_meta_events();
    while (indices.meta_idx < meta_events.size()
        && meta_events[indices.meta_idx].absolute_time <= current_tick) {

        const auto& meta_event = meta_events[indices.meta_idx];
        indices.meta_idx++;

        if (meta_event.type == TEMPO_SETTING) {
            uint32_t mpqn = 0;
            for (auto& byte : meta_event.data) {
                mpqn = (mpqn << 8) | byte;
            }
            micros_per_tick = calculate_mpt(mpqn, track_sequence.get_division());
        }

        // Other meta events largely ignored by this non-GUI program
    }

    // // Sysex event processing
    // const auto& sysex_events = track.get_sysex_events();
    // while (indices.sysex_idx < sysex_events.size()
    //     && sysex_events[indices.sysex_idx].absolute_time <= current_tick) {
    //
    //     const auto& sysex_event = sysex_events[indices.sysex_idx];
    //     indices.sysex_idx++;
    //
    //     // sysex events ignored by sequencer?
    // }
}

[[nodiscard]] bool MidiSequencer::has_more_events() const {
    auto& tracks = track_sequence.get_tracks();
    for (int i = 0; i < tracks.size(); i++) {
        auto& track = tracks[i];
        auto& indices = track_indices[i];

        if (indices.note_idx < track.get_notes().size()) {
            return true;
        }
        else if (indices.midi_event_idx < track.get_midi_events().size()) {
            return true;
        }
        else if (indices.meta_idx < track.get_meta_events().size()) {
            return true;
        }
        else if (indices.sysex_idx < track.get_sysex_events().size()) {
            return true;
        }
    }

    return false;
}

void MidiSequencer::start() {
    is_playing_flag = true;
}

void MidiSequencer::stop() {
    is_playing_flag = false;
}

void MidiSequencer::update() {
    if (!is_playing_flag) return;
    const auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed_micros = std::chrono::duration_cast<std::chrono::microseconds>(current_time - prev_tick_time).count();

    if (midi_file_ended_flag) return;

    // Remove expired notes
    while (!active_notes.empty() && active_notes.top().end_time <= current_tick) {
        auto& note = active_notes.top();
        synthesizer->note_off(note.channel, note.pitch);
        active_notes.pop();
    }

    // Add new notes by triggering events registered to the current tick
    while (elapsed_micros >= micros_per_tick) {
        prev_tick_time += std::chrono::microseconds(micros_per_tick);
        current_tick++;
        elapsed_micros -= micros_per_tick;

        // Go through each "track" (each instrument/part)
        const auto& tracks = track_sequence.get_tracks();
        for (int i = 0; i < tracks.size(); i++) {
            const auto& track = tracks[i];
            auto& indices = track_indices[i];
            process_events(track, indices);
        }
    }

    // Allow early return if the song is over
    if (!has_more_events() && active_notes.empty()) {
        midi_file_ended_flag = true;
    }
}

void MidiSequencer::reset() {
    is_playing_flag = false;
    init();
}

const TrackSequence& MidiSequencer::get_track_sequence() const {
    return track_sequence;
}

void MidiSequencer::set_track_sequence(const TrackSequence& track_sequence) {
    this->track_sequence = track_sequence;
}

bool MidiSequencer::is_playing() const {
    return is_playing_flag;
}

bool MidiSequencer::midi_file_ended() const {
    return midi_file_ended_flag;
}