#ifndef MIDI_PARSERSYNTHESIZER_MIDISEQUENCER_H
#define MIDI_PARSERSYNTHESIZER_MIDISEQUENCER_H
#include <chrono>
#include <queue>

#include "ActiveNote.h"
#include "TrackIndices.h"
#include "../Containers/TrackSequence.h"


class MidiSequencer {
private:
    TrackSequence track_sequence;
    bool is_playing_flag;

    // Helper variables for timing.
    uint32_t micros_per_tick;
    uint32_t current_tick;
    std::chrono::high_resolution_clock::time_point prev_tick_time;
    uint64_t micros_since_last_tick;

    // Helper variables for tracking.
    // Should never be modified with external mutator methods.
    std::vector<TrackIndices> track_indices;
    std::priority_queue<ActiveNote, std::vector<ActiveNote>, std::greater<>> active_notes;

    bool midi_file_ended_flag;

    void init();
    [[nodiscard]] inline uint32_t calculate_micros_per_tick(const uint32_t& tempo) const;
    void process_events(const Track& track, TrackIndices& indices);
    [[nodiscard]] bool has_more_events();

public:
    MidiSequencer();
    MidiSequencer(const TrackSequence& track_sequence);
    MidiSequencer(const MidiSequencer& other);

    void start();
    void stop();
    void update();
    void reset();

    /**
     * Retrieves the internal @c TrackSequence object.
     *
     * @return A constant reference to the internal @c TrackSequence object.
     */
    [[nodiscard]] const TrackSequence& get_track_sequence() const;

    /**
     * Sets the internal @c TrackSequence object to the given
     * @code track_sequence@endcode.
     *
     * @details @code this.reset()@endcode should be called immediately
     * after this method executes, else unexpected behavior may occur.
     *
     * @param track_sequence the @c TrackSequence object to
     * be set as the internal track sequence.
     */
    void set_track_sequence(const TrackSequence& track_sequence);

    /**
     * Indicates whether the sequencer is currently in the playing state.
     *
     * @return A boolean value, where @c true indicates that the sequencer
     * is currently playing, and @c false indicates it is not.
     */
    [[nodiscard]] bool get_is_playing() const;

    /**
     * Indicates if the sequence has reached the end of the parsed
     * midi file.
     *
     * @return a boolean value, where @c true indicates that the
     * sequencer has reached the end of the parsed midi data, and
     * @c false indicates the opposite.
     */
    [[nodiscard]] bool midi_file_ended() const;
};


#endif //MIDI_PARSERSYNTHESIZER_MIDISEQUENCER_H