#ifndef MIDI_PARSERSYNTHESIZER_MIDIPARSER_H
#define MIDI_PARSERSYNTHESIZER_MIDIPARSER_H

#include <cstdint>
#include <string>
#include <vector>
#include "File.h"


class MidiParser {
private:
    File file;

    // Header data:
    // format--0 for single track, 1 for multi-track sync, 2 for multi-track async
    uint16_t format = 0;
    // number of tracks
    uint16_t num_tracks = 0;
    // ticks per quarter note
    uint16_t division = 0;

    // Current byte pos
    std::size_t cursor = 0;

    // Helper functions for Big-Endian format of midi files
    uint16_t read_uint16();
    uint32_t read_uint32();
    std::string read_string(std::size_t length);

public:
    MidiParser();
    MidiParser(const File& file);
    MidiParser(const MidiParser& other);

    bool parse();
};

#endif //MIDI_PARSERSYNTHESIZER_MIDIPARSER_H