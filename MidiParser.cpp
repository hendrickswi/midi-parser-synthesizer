#include "MidiParser.h"

#include <iostream>
#include <ostream>

#include "File.h"
#include "TrackSequence.h"

using namespace std;

// Constructors
MidiParser::MidiParser() = default;

MidiParser::MidiParser(const File& file) {
    this->file = file;
}
MidiParser::MidiParser(const MidiParser& other) {
    this->file = other.file;
}

uint16_t MidiParser::read_uint16() {
    const auto& data = file.get_data();
    if (cursor + 2 > data.size()) {
        return 0;
    }

    // Bitwise OR for combining the bytes
    // Also Big Endian to Little Endian conversion
    uint16_t result = data.at(cursor) << 8 | data.at(cursor + 1);
    cursor += 2;
    return result;
}

uint32_t MidiParser::read_uint32() {
    const auto& data = file.get_data();
    if (cursor + 4 > data.size()) {
        return 0;
    }

    // Bitwise OR for combining the bytes
    // Also Big Endian to Little Endian conversion
    uint32_t result = data.at(cursor) << 24 | data.at(cursor + 1) << 16 | data.at(cursor + 2) << 8 | data.at(cursor + 3);
    cursor += 4;
    return result;
}

std::string MidiParser::read_string(std::size_t length) {
    const auto& data = file.get_data();
    if (cursor + length > data.size()) {
        return "";
    }

    // Range (iterator) constructor
    string result(data.begin() + cursor, data.begin() + cursor + length);
    cursor += length;
    return result;
}

uint32_t MidiParser::read_vlq() {
    uint32_t value = 0;
    uint8_t byte;
    const auto& data = file.get_data();

    do {
        // Prevent out-of-bounds
        if (cursor >= data.size()) return 0;

        byte = data.at(cursor);
        cursor++;

        // Shift previous bits left by 7, and merge the bottom 7 bits of the current byte
        value = (value << 7) | (byte & 0x7F);
    }
    // Continue if the top bit is 1 (escape bit)
    while (byte & 0x80);

    return value;
}

bool MidiParser::parse_midi_event(Track& track, const uint32_t& current_time) {
    // TODO: implement this
}

bool MidiParser::parse_meta_event(Track& track, const uint32_t& current_time) {
    // TODO: implement this
}

bool MidiParser::parse_sysex_event(Track& track, const uint32_t& current_time) {
    // TODO: implement this
}

bool MidiParser::parse_track_event(Track& track, uint32_t& current_time, uint8_t& running_status) {
    // Every track event starts with a delta time
    // Add it to the total time to allow events to be instantiated with absolute time
    uint32_t delta_time = read_vlq();
    current_time += delta_time;

    // Account for "running status"
    uint8_t peek_byte = file.get_data().at(cursor);
    if (peek_byte >= 0x80) {
        // New status
        running_status = peek_byte;
        cursor++;
    }

    // Dispatch to the correct event parser method
    if (peek_byte == 0xFF) {
        // Meta event
        cursor++;
        return parse_meta_event(track, current_time);
    }
    else if (peek_byte == 0xF0 || peek_byte == 0xF7) {
        // Sysex event
        cursor++;
        return parse_sysex_event(track, current_time);
    }
    else if (peek_byte >= 0x80 && peek_byte <= 0xEF) {
        // Midi event
        // Do not do cursor++ here, parse_midi_event needs to know how many data bytes to read
        parse_midi_event(track, current_time);
    }
    else {
        return false;
    }
}

bool MidiParser::parse_track_chunk(Track& track, const long& num_bytes) {
    uint32_t current_time = 0;
    uint8_t running_status = 0;

    while (cursor < cursor + num_bytes) {
        bool success = parse_track_event(track, current_time, running_status);
        if (!success) {
            cerr << "Error: Unable to parse track event at byte " << cursor << endl;
            return false;
        }
    }
    return true;
}

bool MidiParser::parse(TrackSequence& sequence) {
    sequence.clear_tracks();

    // Header data (to be used to edit the given sequence):
    // format--0 for single track, 1 for multi-track sync, 2 for multi-track async
    uint16_t format = 0;
    // number of tracks
    uint16_t num_tracks = 0;
    // ticks per quarter note
    uint16_t division = 0;

    // Attempt to load the file into memory
    bool load = file.load_file();
    if (!load) {
        cerr << "Error: Unable to load the file " << file.get_file_path() << endl;
        return false;
    }

    // Determine if the file has the first MThd signature
    string first_signature = read_string(4);
    if (first_signature != "MThd") {
        cerr << "Error: The file is not a valid MIDI file. Does not have the first MThd signature" << endl;
        return false;
    }

    // Get the header length (size of the next three fields)
    uint32_t header_length = read_uint32();
    if (header_length != 6) {
        cerr << "Error: Header length is " << header_length <<
            " bytes, not the normal 6 bytes. This program only supports 6 byte headers." << endl;
        return false;
    }

    // Get information about this MIDI file
    format = read_uint16();
    num_tracks = read_uint16();
    division = read_uint16();
    if (format != 0 && format != 1) {
        cerr << "Error: format is " << format <<
            ". This program only supports formats 0 and 1 (single track and multiple track)." << endl;
    }
    if (division <= 0) {
        cerr << "Error: delta timing is " << division <<
                " ticks per beat. This program does not support SMPTE-compatible units." << endl;
        return false;
    }

    for (int i = 0; i < num_tracks; i++) {
        string track_signature = read_string(4);
        if (track_signature != "MTrk") {
            cerr << "Error: Track " << i << " is not a valid MIDI track. "
                << "Does not have the MTrk signature OR num_tracks from the header "
                << "chunk is greater than the actual number of tracks in this MIDI file." << endl;
            return false;
        }

        Track track = Track();
        uint32_t chunk_length = read_uint32();
        parse_track_chunk(track, chunk_length);
        sequence.add_track(track);
    }

    // Check that there are no extra track chunks (i.e., num_tracks < actual number of tracks)
    if (cursor != file.get_data().size()) {
        cerr << "Warning: There are " << file.get_data().size() - cursor
            << " bytes left in " << file.get_file_path() << endl;

        if (read_string(4) != "MTrk") {
            cerr << "Info: Remaining bytes do not constitute another track chunk." << endl;
            cerr << "Error: This is not a valid MIDI file. Garbage data after last track." << endl;
            return false;
        }
        else {
            cerr << "Info: Remaining bytes constitute another track chunk." << endl;
            cerr << "Warning: Not reading additional track chunks after track number " << num_tracks << endl;
        }
    }

    return true;
}