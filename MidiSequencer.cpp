#include "MidiSequencer.h"

#include "EventTypeEnums/MetaEventType.h"

constexpr int microseconds_in_a_minute = 60000000;

void MidiSequencer::init() {
    current_tick = 0;
    prev_tick_time = std::chrono::high_resolution_clock::now();
    micros_since_last_tick = 0;
}

inline uint32_t MidiSequencer::calculate_micros_per_tick(const uint32_t& tempo) const {
    return microseconds_in_a_minute / tempo / track_sequence.get_division();
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

    while (micros_since_last_tick >= micros_per_tick) {
        micros_since_last_tick -= micros_per_tick;
        current_tick++;

        // TODO: Generalize this so it works for all types of events
        const auto& tracks = track_sequence.get_tracks();
        for (int i = 0; i < tracks.size(); i++) {
            const auto& meta_events = tracks[i].get_meta_events();
            for (int j = 0 ; j < meta_events.size(); j++) {
                const MetaEvent& meta_event = meta_events[j];
                if (meta_event.absolute_time == current_tick && meta_event.type == TEMPO_SETTING) {
                    // Values are already in Little-Endian format from the parser
                    uint32_t tempo = meta_event.data.at(0) | meta_event.data.at(1) << 8
                        | meta_event.data.at(2) << 16;
                    micros_per_tick = calculate_micros_per_tick(tempo);
                }
            }
        }
    }
}

void MidiSequencer::reset() {
    is_playing = false;
    init();
}

TrackSequence& MidiSequencer::get_track_sequence() {
    return track_sequence;
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