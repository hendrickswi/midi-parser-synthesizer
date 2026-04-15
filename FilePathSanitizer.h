#ifndef MIDI_PARSERSYNTHESIZER_FILEPATHSANITIZER_H
#define MIDI_PARSERSYNTHESIZER_FILEPATHSANITIZER_H
#include <string>

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
void sanitize_file_path(std::string &file_path);

#endif //MIDI_PARSERSYNTHESIZER_FILEPATHSANITIZER_H