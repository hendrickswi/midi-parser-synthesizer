#ifndef MIDI_PARSERSYNTHESIZER_TRACKSEQUENCE_H
#define MIDI_PARSERSYNTHESIZER_TRACKSEQUENCE_H

#include <vector>
#include "Track.h"

class TrackSequence {
private:
    // Each MIDI file can only have one division but multiple tracks
    std::vector<Track> tracks = std::vector<Track>();
    uint16_t division;

public:
    TrackSequence();
    TrackSequence(const std::vector<Track>& tracks, uint16_t division);
    TrackSequence(const TrackSequence& other);

    [[nodiscard]] const std::vector<Track>& get_tracks() const;
    void add_track(const Track& track);
    void clear_tracks();
    [[nodiscard]] uint16_t get_division() const;
    void set_division(uint16_t division);
};

#endif //MIDI_PARSERSYNTHESIZER_TRACKSEQUENCE_H