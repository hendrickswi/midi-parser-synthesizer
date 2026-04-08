#include <iostream>
#include <fstream>
#include "MidiParser.h"

int main() {
    std::cout << "MIDI parser and synthesizer program" << std::endl << std::endl;

    // Get file name
    std::cout << "Enter the file name: ";
    std::string file_name;
    std::cin >> file_name;
    std::cout << std::endl;

    // Load the midi file and parse it
    MidiParser parser = MidiParser(File(file_name));
    TrackSequence tracks = TrackSequence();
    parser.parse(tracks);

    // Play the parsed midi file
    // TODO: Implement this

    return 0;
}