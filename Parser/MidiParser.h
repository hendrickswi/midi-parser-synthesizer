#ifndef MIDI_PARSERSYNTHESIZER_MIDIPARSER_H
#define MIDI_PARSERSYNTHESIZER_MIDIPARSER_H

#include <cstdint>
#include <string>
#include <vector>
#include "../Containers/File.h"
#include "../Containers/TrackSequence.h"

class MidiParser {
private:
    File file;

    // Current byte pos
    std::size_t cursor;

    /*
     * Helper collections for keeping track of active notes.
     * Each channel (outside vector) has 128 pitches (inside vector).
     * Allows notes to be created on a rolling basis.
     * Explicitly does not allow duplicates.
     */
    std::vector<std::vector<uint32_t>> active_note_start_times;
    std::vector<std::vector<uint32_t>> active_note_volumes;

    // Helper functions for Big-Endian format of midi files
    uint16_t read_uint16();
    uint32_t read_uint32();
    std::string read_string(std::size_t length);

    // Helper functions for parsing variable length quantities (VLQ)s
    uint32_t read_vlq();

    // Helper functions for parsing the various types of events
    bool parse_midi_event(Track& track, const uint32_t& current_time, const uint8_t& status_byte);
    bool parse_meta_event(Track& track, const uint32_t& current_time, const uint8_t& status_byte);
    bool parse_sysex_event(Track& track, const uint32_t& current_time, const uint8_t& status_byte);

    // Router method for calling above specific event methods
    bool parse_track_event(Track& track, uint32_t& current_time, uint8_t& running_status);

    // Helper function for parsing a singular track chunk
    bool parse_track_chunk(Track& track, const long& num_bytes);

public:
    MidiParser();
    MidiParser(const File& file);
    MidiParser(const MidiParser& other);

    /**
    * Parses the file which was given during instantiation of @code this@endcode.
    * Puts parsed tracks in @code sequence@endcode and returns a bool indicating success.
    *
    * @param sequence the sequence to insert tracks into
    * @return a bool representing the success of parsing
    */
    bool parse(TrackSequence& sequence);
};

#endif //MIDI_PARSERSYNTHESIZER_MIDIPARSER_H