#include "PlaylistNavigator.h"

#include <algorithm>
#include <iostream>

bool PlaylistNavigator::is_file_loaded(const std::string& file_path) {
    return std::find(loaded_files.begin(), loaded_files.end(), file_path) != loaded_files.end();
}

PlaylistNavigator::PlaylistNavigator(const std::vector<std::string>& loaded_files) {
    this->loaded_files = loaded_files;
    history_stack = std::stack<std::size_t>();
    future_stack = std::stack<std::size_t>();
    current_idx = 0;
    has_active_track = false;
    shuffle_flag = false;
    repeat_flag = false;
}

std::size_t PlaylistNavigator::get_next_idx(bool forced_skip) const {
    if (repeat_flag && !forced_skip) {
        return current_idx;
    }

    if (!future_stack.empty()) {
        return future_stack.top();
    }

    if (shuffle_flag && !forced_skip) {
        std::size_t idx = std::rand() % loaded_files.size();
        if (history_stack.size() <= 1) return idx;

        // Prevent the same file from being played again if there are options
        while (idx == history_stack.top()) {
            idx = std::rand() % loaded_files.size();
        }

        return idx;
    }

    std::size_t candidate = current_idx + 1;
    if (candidate >= loaded_files.size()) {
        candidate = 0;
    }
    return candidate;
}

std::size_t PlaylistNavigator::get_previous_idx() const {
    if (!history_stack.empty()) {
        return history_stack.top();
    }
    return has_active_track ? current_idx : 0;
}

bool PlaylistNavigator::commit_to_history(std::size_t new_idx, NavigationDirection direction) {
    if (new_idx >= loaded_files.size() || (new_idx == current_idx && has_active_track)) return false;

    if (has_active_track) {
        switch (direction) {
            case NavigationDirection::FORWARD : {
                history_stack.push(current_idx);

                // Case: moving forward after having skipped backward
                if (!future_stack.empty() && future_stack.top() == new_idx) {
                    future_stack.pop();
                }
                else {
                    // Clear the future stack as it no longer represents the current future
                    while (!future_stack.empty()) {
                        future_stack.pop();
                    }
                }
                break;
            }
            case NavigationDirection::BACKWARD : {
                future_stack.push(current_idx);

                // Case: moving backward in playback history
                if (!history_stack.empty() && history_stack.top() == new_idx) {
                    history_stack.pop();
                }
                break;
            }
            case NavigationDirection::JUMP_TO : {
                history_stack.push(current_idx);

                // Clear the now inaccurate future
                while (!future_stack.empty()) {
                    future_stack.pop();
                }
                break;
            }
            default : {
                std::cerr << "WARNING: Unhandled navigation direction in PlaylistNavigator::commit_to_history()" << std::endl;
                break;
            }
        }
    }

    current_idx = new_idx;
    has_active_track = true;
    return true;
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