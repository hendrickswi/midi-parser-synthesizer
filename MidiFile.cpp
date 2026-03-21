#include "MidiFile.h"

#include <iostream>
#include <fstream>

using namespace std;

// Constructors
MidiFile::MidiFile() = default;

MidiFile::MidiFile(const string& file_path) {
    this->file_path = file_path;
}

MidiFile::MidiFile(const MidiFile& other) {
    this->data = other.data;
    this->file_path = other.file_path;
}

// Accessor and mutator methods
std::vector<uint8_t>& MidiFile::get_data() {
    return data;
}

void MidiFile::set_data(const vector<uint8_t>& data) {
    this->data = data;
}

std::string MidiFile::get_file_path() {
    return file_path;
}

void MidiFile::set_file_path(const string& file_path) {
    this->file_path = file_path;
}

// Other methods
bool MidiFile::load_midi_file() {
    data.clear();

    // Open file with instance variable file_path, return false if unable to open
    ifstream fin(file_path, ios::binary);
    if (!fin.is_open()) {
        cerr << "Unable to open the file " << file_path << endl;
        return false;
    }

    // Read file and populate data instance variable
    fin.seekg(0, ios::end);
    std::streamsize size = fin.tellg();
    fin.seekg(0, ios::beg);

    data.resize(size);
    // data.data() used to provide access to the underlying contiguous array
    fin.read((char*)data.data(), size);
    if (!fin) {
        cerr << "Unable to read the file " << file_path << endl;
        return false;
    }

    fin.close();
    return true;
}

bool is_midi_file(const vector<uint8_t>& midi_data) {
    if (midi_data.size() < 4) {
        return false;
    }

    // Check that the midi file has the proper start
    return midi_data.at(0) == 'M' && midi_data.at(1) == 'T' &&
        midi_data.at(2) == 'h' && midi_data.at(3) == 'd';
}
