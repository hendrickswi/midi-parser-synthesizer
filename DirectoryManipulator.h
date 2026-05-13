#ifndef MIDI_PARSERSYNTHESIZER_FILEPATHSANITIZER_H
#define MIDI_PARSERSYNTHESIZER_FILEPATHSANITIZER_H
#include <string>
#include <vector>

class File;

class DirectoryManipulator {
private:
    std::string current_directory_path;

public:
    DirectoryManipulator();
    DirectoryManipulator(std::string directory_path);
    DirectoryManipulator(const DirectoryManipulator& other);

    void set_current_directory_path(std::string file_path);
    std::string get_current_directory_path() const;
    std::vector<std::string> get_midi_file_paths_in_directory() const;
    std::vector<File> get_midi_files_in_directory() const;
};

/**
 * Sanitizes a given file path by replacing directory separators with the
 * appropriate format based on the operating system.
 *
 * On Windows, this will replace forward slashes ('/') with backslashes ('\').
 * On non-Windows systems, this will replace backslashes ('\') with forward slashes ('/').
 *
 * @param file_path The file path to be sanitized. This parameter is modified
 * in place to reflect the sanitized path.
 */
void sanitize_file_path(std::string& file_path);

#endif //MIDI_PARSERSYNTHESIZER_FILEPATHSANITIZER_H