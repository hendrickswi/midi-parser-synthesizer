#include "MidiParser.h"
#include "MidiFile.h"

using namespace std;

// Constructors
MidiParser::MidiParser() = default;

MidiParser::MidiParser(const MidiFile& file) {
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
    uint32_t result = data.at(cursor) << 24 | data.at(cursor + 1) << 16 | data.at(cursor + 2) << 8 | data.at(cursor + 3);
    cursor += 4;
    return result;
}

std::string MidiParser::read_string(std::size_t length) {

}

bool MidiParser::parse() {
    bool load = this->file.load_midi_file();
    if (!load) {
        return false;
    }

    bool result = is_midi_file(this->file.get_data());
    if (!result) {
        return false;
    }

    return true;
}