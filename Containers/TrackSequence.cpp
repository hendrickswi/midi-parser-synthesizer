#include "TrackSequence.h"

TrackSequence::TrackSequence() {
    this->division = 0;
}

TrackSequence::TrackSequence(const std::vector<Track>& tracks, uint16_t division) {
    for (const Track& track : tracks) {
        this->tracks.push_back(track);
    }
    this->division = division;
}

TrackSequence::TrackSequence(const TrackSequence& other) {
    tracks = other.tracks;
    division = other.division;
}


[[nodiscard]] uint16_t TrackSequence::get_division() const {
    return division;
}

void TrackSequence::add_track(const Track& track) {
    tracks.push_back(track);
}

void TrackSequence::clear_tracks() {
    tracks.clear();
}

[[nodiscard]] const std::vector<Track>& TrackSequence::get_tracks() const {
    return tracks;
}

void TrackSequence::set_division(uint16_t division) {
    this->division = division;
}