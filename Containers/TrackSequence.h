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

    void clear_tracks();
    void add_track(const Track& track);
    void set_division(uint16_t division);
};

#endif //MIDI_PARSERSYNTHESIZER_TRACKSEQUENCE_H