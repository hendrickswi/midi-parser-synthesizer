#include "MidiSequencer.h"
#include "../EventTypeEnums/MetaEventType.h"
#include "../EventTypeEnums/MidiEventType.h"

[[nodiscard]] inline uint32_t calculate_mpqn(const uint32_t tempo) {
    return 60000000 / tempo;
}

[[nodiscard]] inline uint32_t calculate_mpt(const uint32_t mpqn, const uint16_t division) {
    return mpqn / division;
}

uint32_t parse_mpt_from_tempo_event(const MetaEvent& tempo_event, uint16_t division) {
    if (tempo_event.type != TEMPO_SETTING) return 0;

    uint32_t mpqn = 0;
    for (auto& byte : tempo_event.data) {
        mpqn = (mpqn << 8) | byte;
    }
    return calculate_mpt(mpqn, division);
}

MidiSequencer::MidiSequencer() { // NOLINT
    track_sequence = nullptr;
    synthesizer = nullptr;

    // Defer init() call until set_track_sequence() in this case
}

MidiSequencer::MidiSequencer(TrackSequence* track_sequence, VoiceManager* synthesizer) { // NOLINT
    this->track_sequence = track_sequence;
    this->synthesizer = synthesizer;
    init();
}

MidiSequencer::MidiSequencer(const MidiSequencer& other) {
    track_sequence = other.track_sequence;
    synthesizer = other.synthesizer;
    is_playing_flag = other.is_playing(); // Assignment operator '=' is removed for std::atomic
    midi_file_ended_flag = other.midi_file_ended_flag;
    micros_per_tick = other.micros_per_tick;
    current_tick = other.current_tick;
    prev_tick_time = other.prev_tick_time;
    micros_since_last_tick = other.micros_since_last_tick;
    track_indices = other.track_indices;
}

void MidiSequencer::init() {
    is_playing_flag = false;
    is_skipping_flag = false;
    midi_file_ended_flag = false;
    current_tick = 0;
    prev_tick_time = std::chrono::high_resolution_clock::now();
    micros_since_last_tick = 0;
    micros_per_tick = calculate_mpt(calculate_mpqn(120), track_sequence->get_division());
    track_indices.clear();

    // This also does default instantiation of TrackIndices structs
    track_indices.resize(track_sequence->get_tracks().size());

    // Cache all tempo change events in order for later usage in multiple methods
    tempo_events = std::vector<MetaEvent>();
    for (const auto& track : track_sequence->get_tracks()) {
        for (const auto& meta_event : track.get_meta_events()) {
            if (meta_event.type == TEMPO_SETTING) {
                tempo_events.push_back(meta_event);
            }
        }
    }
    std::sort(tempo_events.begin(), tempo_events.end(), [](const MetaEvent& a, const MetaEvent& b) {
        return a.absolute_time < b.absolute_time;
    });
}

void MidiSequencer::process_events(const Track& track, TrackIndices& indices) {
    if (!is_skipping_flag) {
        // Note processing
        const auto& notes = track.get_notes();
        while (indices.note_idx < notes.size()
            && notes[indices.note_idx].absolute_time <= current_tick) {

            const auto& note = notes[indices.note_idx];
            indices.note_idx++;
            synthesizer->note_on(note.channel, note.pitch, note.volume);
            active_notes.emplace(note);
        }
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
            micros_per_tick = calculate_mpt(mpqn, track_sequence->get_division());
        }

        // Other meta events largely ignored by this non-GUI program
    }
}

[[nodiscard]] bool MidiSequencer::has_more_events() const {
    auto& tracks = track_sequence->get_tracks();
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
    if (!synthesizer || !track_sequence) return;
    is_playing_flag = true;

    // Reanchor
    prev_tick_time = std::chrono::high_resolution_clock::now();
}

void MidiSequencer::stop() {
    if (!synthesizer || !track_sequence) return;
    is_playing_flag = false;
}

void MidiSequencer::skip_forward(float seconds) {
    // Instead of complex state-rebuilding logic, just simulate the passage of time

    synthesizer->stop();
    while (!active_notes.empty()) active_notes.pop();
    is_skipping_flag = true;

    const auto& tracks = track_sequence->get_tracks();
    uint32_t micros_to_skip = seconds * 1000000.0f;
    while (micros_to_skip >= micros_per_tick) {
        current_tick++;
        micros_to_skip -= micros_per_tick;

        for (int i = 0; i < tracks.size(); i++) {
            process_events(tracks[i], track_indices[i]);
        }
    }

    is_skipping_flag = false;
    prev_tick_time = std::chrono::high_resolution_clock::now();
}

void MidiSequencer::skip_backward(float seconds) {
    // TODO: Implement this
}

void MidiSequencer::update() {
    if (!is_playing_flag || !synthesizer || !track_sequence) return;
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
        const auto& tracks = track_sequence->get_tracks();
        for (int i = 0; i < tracks.size(); i++) {
            const auto& track = tracks[i];
            auto& indices = track_indices[i];
            process_events(track, indices);
        }
    }

    // Allow early return if the song is over
    if (!has_more_events() && active_notes.empty()) {
        midi_file_ended_flag = true;
        is_playing_flag = false;
    }
}

void MidiSequencer::reset() {
    if (!synthesizer || !track_sequence) return;
    is_playing_flag = false;
    init();
}

const TrackSequence* const MidiSequencer::get_track_sequence() const {
    return track_sequence;
}

void MidiSequencer::set_track_sequence(TrackSequence* sequence) {
    track_sequence = sequence;
    init();
}

const VoiceManager* const MidiSequencer::get_synthesizer() const {
    return synthesizer;
}

void MidiSequencer::set_synthesizer(VoiceManager* synth) {
    synthesizer = synth;
}

bool MidiSequencer::is_playing() const {
    return is_playing_flag;
}

bool MidiSequencer::midi_file_ended() const {
    return midi_file_ended_flag;
}

float MidiSequencer::get_current_time_seconds() const {
    if (!track_sequence || current_tick == 0) return 0.0f;

    // Calculate duration by accumulating based on the current tempo (mpt)
    // Slightly modified version of the algorithm from get_total_duration_seconds--
    // only go until the last event before current_tick
    float elapsed_seconds = 0.0f;
    uint32_t prev_tempo_event_time = 0;
    uint32_t prev_mpt = micros_per_tick;
    for (const auto& tempo_event : tempo_events) {
        if (tempo_event.absolute_time >= current_tick) break;
        if (tempo_event.absolute_time > prev_tempo_event_time) {
            elapsed_seconds += (tempo_event.absolute_time - prev_tempo_event_time) * prev_mpt / 1000000.0f;
        }

        // Setup for next iteration
        prev_tempo_event_time = tempo_event.absolute_time;
        prev_mpt = parse_mpt_from_tempo_event(tempo_event, track_sequence->get_division());
    }

    // Final accumulation
    if (current_tick > prev_tempo_event_time) {
        elapsed_seconds += (current_tick - prev_tempo_event_time) * prev_mpt / 1000000.0f;
    }

    return elapsed_seconds;
}

float MidiSequencer::get_total_duration_seconds() const {
    if (!track_sequence) return 0.0f;

    // Find the end time farthest away from the start
    uint32_t greatest_end_time = 0;
    for (const auto& track : track_sequence->get_tracks()) {
        const auto& notes = track.get_notes();
        if (notes.empty()) continue;

        for (const auto& note : notes) {
            uint32_t end_time = note.absolute_time + note.duration;
            if (end_time > greatest_end_time) {
                greatest_end_time = end_time;
            }
        }
    }

    // Calculate duration by accumulating based on the current tempo (mpt)
    float duration_seconds = 0.0f;
    uint32_t prev_tempo_event_time = 0;
    uint32_t prev_mpt = micros_per_tick;
    for (const auto& tempo_event : tempo_events) {
        if (tempo_event.absolute_time > prev_tempo_event_time) {
            duration_seconds += (tempo_event.absolute_time - prev_tempo_event_time) * prev_mpt / 1000000.0f;
        }

        // Set up for next iteration
        prev_tempo_event_time = tempo_event.absolute_time;
        prev_mpt = parse_mpt_from_tempo_event(tempo_event, track_sequence->get_division());
    }

    // Final accumulation
    if (prev_tempo_event_time < greatest_end_time) {
        duration_seconds += (greatest_end_time - prev_tempo_event_time) * prev_mpt / 1000000.0f;
    }

    return duration_seconds;
}