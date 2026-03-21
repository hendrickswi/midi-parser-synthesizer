#include <cstdint>
#include <iostream>
#include <vector>
#include <fstream>
#include "MidiParser.h"

using namespace std;

int main() {
    cout << "MIDI parser and synthesizer program" << endl << endl;

    // Get file name
    cout << "Enter the file name: ";
    string file_name;
    cin >> file_name;
    cout << endl;

    // Load the midi file
    MidiParser parser = MidiParser(MidiFile(file_name));

    return 0;
}