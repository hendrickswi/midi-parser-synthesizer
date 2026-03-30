#include "TrackSequence.h"

TrackSequence::TrackSequence() {
    this->division = 0;
}

TrackSequence::TrackSequence(const std::vector<Track>& tracks, uint16_t division) {
    for (Track track : tracks) {
        this->tracks.push_back(track);
    }
    this->division = division;
}

TrackSequence::TrackSequence(const TrackSequence& other) {
    tracks = other.tracks;
    division = other.division;
}

void TrackSequence::clear_tracks() {
    tracks.clear();
}

void TrackSequence::add_track(const Track& track) {
    tracks.push_back(track);
}

void TrackSequence::set_division(uint16_t division) {
    this->division = division;
}