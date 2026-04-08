#include "File.h"

#include <iostream>
#include <fstream>

using namespace std;

// Constructors
File::File() {
    this->file_path = "";
    this->data = vector<uint8_t>();
}

File::File(const string& file_path) {
    this->file_path = file_path;
    this->data = vector<uint8_t>();
}

File::File(const File& other) {
    this->file_path = other.file_path;
    this->data = other.data;
}

// Accessor and mutator methods
std::vector<uint8_t>& File::get_data() {
    return data;
}

void File::set_data(const vector<uint8_t>& data) {
    this->data = data;
}

std::string File::get_file_path() {
    return file_path;
}

void File::set_file_path(const string& file_path) {
    this->file_path = file_path;
}

// Other methods
bool File::load_file() {
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
