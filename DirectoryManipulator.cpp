#include "DirectoryManipulator.h"
#include <algorithm>
#include <filesystem>
#include <iostream>

#include "Containers/File.h"

DirectoryManipulator::DirectoryManipulator() {
    current_directory_path = "";
}

DirectoryManipulator::DirectoryManipulator(std::string directory_path) {
    current_directory_path = directory_path;
    sanitize_file_path(current_directory_path);
}

DirectoryManipulator::DirectoryManipulator(const DirectoryManipulator& other) {
    current_directory_path = other.current_directory_path;
}

void DirectoryManipulator::set_current_directory_path(std::string file_path) {
    current_directory_path = file_path;
    sanitize_file_path(current_directory_path);
}

std::string DirectoryManipulator::get_current_directory_path() const {
    return current_directory_path;
}

std::vector<std::string> DirectoryManipulator::get_midi_file_paths_in_directory() const {
    std::vector<std::string> file_paths = std::vector<std::string>();
    if (!std::filesystem::exists(current_directory_path) || !std::filesystem::is_directory(current_directory_path)) {
        std::cerr << "Error: The directory " << current_directory_path << " cannot be resolved" << std::endl;
        return file_paths;
    }

    for (const auto& entry : std::filesystem::directory_iterator(current_directory_path)) {
        if (entry.path().extension() == ".mid") {
            file_paths.push_back(entry.path().string());
        }
    }

    return file_paths;
}

std::vector<File> DirectoryManipulator::get_midi_files_in_directory() const {
    auto files = std::vector<File>();
    if (!std::filesystem::exists(current_directory_path) || !std::filesystem::is_directory(current_directory_path)) {
        std::cerr << "Error: The directory " << current_directory_path << " cannot be resolved" << std::endl;
        return files;
    }

    for (const auto& entry : std::filesystem::directory_iterator(current_directory_path)) {
        if (entry.path().extension() == ".mid") {
            files.push_back(File(entry.path().string()));
        }
    }

    return files;
}

void sanitize_file_path(std::string& file_path) {
#ifdef _WIN32
    std::replace(file_path.begin(), file_path.end(), '/', '\\');
#else
    std::replace(file_path.begin(), file_path.end(), '\\', '/');
#endif
}
