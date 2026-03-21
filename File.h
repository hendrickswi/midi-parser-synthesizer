#ifndef MIDI_PARSERSYNTHESIZER_MIDIFILE_H
#define MIDI_PARSERSYNTHESIZER_MIDIFILE_H

#include <vector>
#include <string>
#include <cstdint>

class File {

private:
    std::string file_path;
    std::vector<std::uint8_t> data;

public:
    File();
    File(const std::string& file_path);
    File(const File& other);

    // Accessor and mutator methods
    std::vector<std::uint8_t>& get_data();
    void set_data(const std::vector<uint8_t>& data);
    std::string get_file_path();
    void set_file_path(const std::string& file_path);

    // Other methods
    bool load_file();
};

#endif //MIDI_PARSERSYNTHESIZER_MIDIFILE_H