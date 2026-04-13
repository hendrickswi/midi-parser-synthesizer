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
    uint64_t micros_since_last_tick;

    void init();
    inline uint32_t calculate_micros_per_tick(const uint32_t& tempo) const;

public:
    MidiSequencer();
    MidiSequencer(const TrackSequence& track_sequence);
    MidiSequencer(const MidiSequencer& other);

    void start();
    void stop();
    void update();
    void reset();

    [[nodiscard]] const TrackSequence& get_track_sequence() const;
    [[nodiscard]] TrackSequence& get_track_sequence();
    void set_track_sequence(const TrackSequence& track_sequence);
    [[nodiscard]] bool get_is_playing() const;
};


#endif //MIDI_PARSERSYNTHESIZER_MIDISEQUENCER_H