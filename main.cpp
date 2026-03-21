#include <cstdint>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

bool load_midi_file(const string& file_name, vector<uint8_t>& midi_data);
bool is_midi_file(const vector<uint8_t>& midi_data);


int main() {
    cout << "MIDI parser and synthesizer program" << endl << endl;

    // Get file name
    cout << "Enter the file name: ";
    string file_name;
    cin >> file_name;
    cout << endl;

    // Load the midi file
    vector<uint8_t> midi_data;
    bool loaded = load_midi_file(file_name, midi_data);
    if (!loaded) {
        cerr << "Unable to load the file " << file_name << endl;
        return 1;
    }

    if (!is_midi_file(midi_data)) {
        cerr << "The file " << file_name << " is not a valid midi file." << endl;
        return 1;
    }


    return 0;
}

bool is_midi_file(const vector<uint8_t>& midi_data) {
    if (midi_data.size() < 4) {
        return false;
    }

    // Check that the midi file has the proper start
    return midi_data.at(0) == 'M' && midi_data.at(1) == 'T' && midi_data.at(2) == 'h' && midi_data.at(3) == 'd';
}