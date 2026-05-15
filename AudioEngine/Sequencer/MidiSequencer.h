#ifndef MIDI_PARSERSYNTHESIZER_MIDISEQUENCER_H
#define MIDI_PARSERSYNTHESIZER_MIDISEQUENCER_H
#include <atomic>
#include <chrono>
#include <queue>

#include "ActiveNote.h"
#include "TrackIndices.h"
#include "../../Containers/TrackSequence.h"
#include "../Synthesizer/VoiceManager.h"

constexpr float micros_to_seconds = 1.0f / 1000000.0f;
constexpr float seconds_to_micros = 1000000.0f;

class MidiSequencer {
private:
    TrackSequence* track_sequence;
    VoiceManager* synthesizer;
    std::atomic<bool> is_playing_flag;
    bool is_skipping_flag;
    bool midi_file_ended_flag;

    // Helper variables for timing.
    uint32_t micros_per_tick;
    uint32_t current_tick;
    std::chrono::high_resolution_clock::time_point prev_tick_time;
    uint64_t micros_since_last_tick;
    uint64_t total_elapsed_micros;

    // Helper variables for tracking.
    // Should never be modified with external mutator methods.
    std::vector<TrackIndices> track_indices;
    std::priority_queue<ActiveNote, std::vector<ActiveNote>, std::greater<>> active_notes;

    // Cached tempo change events
    std::vector<MetaEvent> tempo_events;

    void init();
    void process_events(const Track& track, TrackIndices& indices);
    void skip_microseconds(uint64_t micros_to_skip);
    [[nodiscard]] bool has_more_events() const;

public:
    MidiSequencer();
    MidiSequencer(TrackSequence* track_sequence, VoiceManager* synthesizer);
    MidiSequencer(const MidiSequencer& other);

    void start();
    void stop();
    void skip_forward(float seconds);
    void skip_backward(float seconds);
    void update();
    void reset();

    /**
     * Retrieves the current @code TrackSequence@endcode being read by @code this@endcode.
     * @return A constant pointer to a constant @code TrackSequence@endcode.
     */
    [[nodiscard]] const TrackSequence* const get_track_sequence() const;

    /**
     * Sets the track sequence to be invoked by @code this@endcode.
     * @param sequence the new @code TrackSequence@endcode to be used by @code this@endcode.
     */
    void set_track_sequence(TrackSequence* sequence);

    /**
     * Retrieves the current @code VoiceManager@endcode being used by @code this@endcode.
     * @return A constant pointer to a constant @code VoiceManager@endcode.
     */
    [[nodiscard]] const VoiceManager* const get_synthesizer() const;

    /**
     * Sets the synthesizer to be invoked by @code this@endcode.
     * @param synth the new @code VoiceManager@endcode to be used by @code this@endcode.
     */
    void set_synthesizer(VoiceManager* synth);

    /**
     * Indicates whether the sequencer is currently in the playing state.
     *
     * @return A boolean value, where @c true indicates that the sequencer
     * is currently playing, and @c false indicates it is not.
     */
    [[nodiscard]] bool is_playing() const;

    /**
     * Indicates if the sequence has reached the end of the parsed
     * midi file.
     *
     * @return a boolean value, where @c true indicates that the
     * sequencer has reached the end of the parsed midi data, and
     * @c false indicates the opposite.
     */
    [[nodiscard]] bool midi_file_ended() const;

    [[nodiscard]] float get_current_time_seconds() const;
    [[nodiscard]] float get_total_duration_seconds() const;
};


#endif //MIDI_PARSERSYNTHESIZER_MIDISEQUENCER_H