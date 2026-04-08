#include <iostream>
#include <fstream>
#include "MidiParser.h"

int main() {
    std::cout << "MIDI parser and synthesizer program" << std::endl << std::endl;

    // Get file name
    std::cout << "Enter the file name: ";
    std::string file_name;
    std::getline(std::cin, file_name);
    std::cout << std::endl;

    // Load the midi file and parse it
    MidiParser parser = MidiParser(File(file_name));
    TrackSequence tracks = TrackSequence();
    if (!parser.parse(tracks)) {
        std::cerr << "Error: Unable to parse the file" << std::endl;
        return 1;
    }

    // Play the parsed midi file
    // TODO: Implement this

    return 0;
}