#include "File.h"

#include <iostream>
#include <fstream>

// Constructors
File::File() {
    this->file_path = "";
    this->data = std::vector<uint8_t>();
}

File::File(const std::string& file_path) {
    this->file_path = file_path;
    this->data = std::vector<uint8_t>();
}

File::File(const File& other) {
    this->file_path = other.file_path;
    this->data = other.data;
}

// Accessor and mutator methods
std::vector<uint8_t>& File::get_data() {
    return data;
}

void File::set_data(const std::vector<uint8_t>& data) {
    this->data = data;
}

std::string File::get_file_path() {
    return file_path;
}

void File::set_file_path(const std::string& file_path) {
    this->file_path = file_path;
}

// Other methods
bool File::load_file() {
    data.clear();

    // Open file with instance variable file_path, return false if unable to open
    std::ifstream fin(file_path, std::ios::binary);
    if (!fin.is_open()) {
        std::cerr << "Unable to open the file " << file_path << std::endl;
        return false;
    }

    // Read file and populate data instance variable
    fin.seekg(0, std::ios::end);
    std::streamsize size = fin.tellg();
    fin.seekg(0, std::ios::beg);

    data.resize(size);
    // data.data() used to provide access to the underlying contiguous array
    fin.read((char*)data.data(), size);
    if (!fin) {
        std::cerr << "Unable to read the file " << file_path << std::endl;
        return false;
    }

    fin.close();
    return true;
}
