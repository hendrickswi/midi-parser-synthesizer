#include "PlaylistNavigator.h"

#include <algorithm>

bool PlaylistNavigator::is_file_loaded(const std::string& file_path) {
    return std::find(loaded_files.begin(), loaded_files.end(), file_path) != loaded_files.end();
}

PlaylistNavigator::PlaylistNavigator(const std::vector<std::string>& loaded_files) {
    this->loaded_files = loaded_files;
    playback_history = std::vector<std::size_t>();
    shuffle_flag = false;
    repeat_flag = false;
}

std::size_t PlaylistNavigator::get_next_idx(bool forced_skip) {
    std::size_t return_idx = 0;
    if (repeat_flag && !forced_skip) {
        return_idx = playback_history.empty() ? 0 : playback_history.back();
    }
    else if (shuffle_flag) {
        return_idx = std::rand() % loaded_files.size();
        if (!playback_history.empty()) {
            while (return_idx == playback_history.back()) {
                return_idx = std::rand() % loaded_files.size();
            }
        }
    }
    else {
        return_idx = (playback_history.empty() ? 0 : playback_history.back()) + 1;
        if (return_idx >= loaded_files.size()) {
            return_idx = 0;
        }
    }

    return return_idx;
}

std::size_t PlaylistNavigator::get_previous_idx() {
    std::size_t return_idx = 0;
    if (!playback_history.empty()) {
        return_idx = playback_history.back();
        playback_history.pop_back();
    }
    else {
        return_idx = 0;
    }

    return return_idx;
}

bool PlaylistNavigator::commit_to_history(std::size_t idx) {
    if (playback_history.empty() || idx != playback_history.back()) {
        playback_history.push_back(idx);
        return true;
    }
    return false;
}

void PlaylistNavigator::add_to_loaded_files(const std::string& file_path) {
    if (!is_file_loaded(file_path)) {
        loaded_files.push_back(file_path);
    }
}

void PlaylistNavigator::set_shuffle(bool enabled) {
    shuffle_flag = enabled;
}

void PlaylistNavigator::set_repeat(bool enabled) {
    repeat_flag = enabled;
}

bool PlaylistNavigator::get_shuffle() const {
    return shuffle_flag;
}

bool PlaylistNavigator::get_repeat() const {
    return repeat_flag;
}