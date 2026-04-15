#include "FilePathSanitizer.h"
#include <algorithm>

void sanitize_file_path(std::string& file_path) {
#ifdef _WIN32
    std::replace(file_path.begin(), file_path.end(), '/', '\\');
#else
    std::replace(file_path.begin(), file_path.end(), '\\', '/');
#endif
}