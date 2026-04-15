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

    [[nodiscard]] const std::vector<std::uint8_t>& get_data() const;
    [[nodiscard]] std::vector<std::uint8_t>& get_data();
    void set_data(const std::vector<uint8_t>& data);

    [[nodiscard]] const std::string& get_file_path() const;
    void set_file_path(const std::string& file_path);

    /**
     * Loads the file from @code this.file_path@endcode and
     * puts the bytes into @code this.data@endcode.
     * @return bool indicating success of file loading
     */
    bool load_file();
};

#endif //MIDI_PARSERSYNTHESIZER_MIDIFILE_H