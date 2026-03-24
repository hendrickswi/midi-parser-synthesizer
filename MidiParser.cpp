#include "MidiParser.h"

#include <iostream>
#include <ostream>

#include "File.h"

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

bool MidiParser::parse_track_chunk(long num_bytes) {

}

bool MidiParser::parse() {
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

        string chunk_length = read_string(4);
        long chunk_length_int = stol(chunk_length);
        parse_track_chunk(chunk_length_int);
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