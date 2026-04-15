#include <filesystem>
#include <iostream>
#include <fstream>
#include "Parser/MidiParser.h"

int main() {
    auto file_names = std::vector<std::string>();
    std::cout << "MIDI parser and synthesizer program" << std::endl << std::endl;

    // Determine if user wants an automatic test or user input test
    std::cout << "Automatic test (1) or user input test (2)?";
    int test_type;
    std::cin >> test_type;
    std::cout << std::endl;

    if (test_type == 2) {
        // Get file name
        std::cout << "Enter the file name: ";
        std::string file_name;
        std::getline(std::cin >> std::ws, file_name);
        std::cout << std::endl;
    }
    else {
        // Get all the .mid file names from '/Testing files' directory
        // TODO: Find a way to sanitize this file path (maybe making File::sanitize_file_path() a friend function?)
        for (const auto& entry : std::filesystem::directory_iterator("../Testing files")) {
            if (entry.path().extension() == ".mid") {
                file_names.push_back(entry.path().string());
            }
        }
    }

    MidiParser parser = MidiParser();
    for (int i = 0; i < file_names.size(); i++) {
        parser.set_file(file_names[i]);
        std::cout << "Parsing file: " << file_names[i] << std::endl;
        auto sequence = TrackSequence();
        if (!parser.parse(sequence)) {
            std::cerr << "Error: Unable to parse the file" << std::endl;
            return 1;
        }
    }

    // Play the parsed midi file
    // TODO: Implement this

    return 0;
}