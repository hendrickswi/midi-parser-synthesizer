#include "MidiSequencer.h"

MidiSequencer::MidiSequencer() {
    track_sequence = TrackSequence();
    is_playing = false;

    micros_per_tick = 500000 / track_sequence.get_division();
    current_tick = 0;
    prev_tick_time = std::chrono::high_resolution_clock::now();
    micros_since_last_tick = 0;
}

MidiSequencer::MidiSequencer(const TrackSequence& track_sequence) {
    this->track_sequence = track_sequence;
    is_playing = false;

    micros_per_tick = 500000 / track_sequence.get_division();
    current_tick = 0;
    prev_tick_time = std::chrono::high_resolution_clock::now();
    micros_since_last_tick = 0;
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
    // TODO: Implement this
}

void MidiSequencer::stop() {
    // TODO: Implement this
}

void MidiSequencer::update() {
    // TODO: Implement this
}

void MidiSequencer::reset() {
    is_playing = false;
    current_tick = 0;
    prev_tick_time = std::chrono::high_resolution_clock::now();
    micros_since_last_tick = 0;
}

TrackSequence& MidiSequencer::get_track_sequence() {
    return track_sequence;
}

void MidiSequencer::set_track_sequence(const TrackSequence& track_sequence) {
    this->track_sequence = track_sequence;
}

bool MidiSequencer::get_is_playing() {
    return is_playing;
}