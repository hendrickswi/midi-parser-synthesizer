#include "File.h"
#include <algorithm>
#include <iostream>
#include <fstream>

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

void File::sanitize_file_path() {
#ifdef _WIN32
    std::replace(file_path.begin(), file_path.end(), '/', '\\');
#else
    std::replace(file_path.begin(), file_path.end(), '\\', '/');
#endif
}

const std::vector<uint8_t>& File::get_data() const {
    return data;
}

std::vector<uint8_t>& File::get_data() {
    return data;
}

void File::set_data(const std::vector<uint8_t>& data) {
    this->data = data;
}

const std::string& File::get_file_path() const {
    return file_path;
}

void File::set_file_path(const std::string& file_path) {
    this->file_path = file_path;
}

bool File::load_file() {
    data.clear();

    // Open file with instance variable file_path, return false if unable to open
    sanitize_file_path();
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
