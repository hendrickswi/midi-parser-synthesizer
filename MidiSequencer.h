#ifndef MIDI_PARSERSYNTHESIZER_MIDISEQUENCER_H
#define MIDI_PARSERSYNTHESIZER_MIDISEQUENCER_H
#include <chrono>

#include "Containers/TrackSequence.h"


class MidiSequencer {
private:
    // Private internal variables that can be edited by public
    // mutator methods.
    TrackSequence track_sequence;
    bool is_playing;

    // Private helper variables for sequencing.
    // Should never be modified with public mutator methods.
    uint32_t micros_per_tick;
    uint32_t current_tick;
    std::chrono::high_resolution_clock::time_point prev_tick_time;
    double micros_since_last_tick;

public:
    MidiSequencer();
    MidiSequencer(const TrackSequence& track_sequence);
    MidiSequencer(const MidiSequencer& other);

    void start();
    void stop();
    void update();
    void reset();

    TrackSequence& get_track_sequence();
    void set_track_sequence(const TrackSequence& track_sequence);
    bool get_is_playing();
};


#endif //MIDI_PARSERSYNTHESIZER_MIDISEQUENCER_H