#ifndef MIDI_PARSERSYNTHESIZER_MIDIPARSER_H
#define MIDI_PARSERSYNTHESIZER_MIDIPARSER_H

#include <cstdint>
#include <string>
#include <vector>
#include "MidiFile.h"


class MidiParser {
public:
    MidiParser();
    MidiParser(const MidiFile& file);
    MidiParser(const MidiParser& other);

    bool parse();

private:
    MidiFile file;


};


#endif //MIDI_PARSERSYNTHESIZER_MIDIPARSER_H