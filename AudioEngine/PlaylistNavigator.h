#ifndef MIDI_PARSERSYNTHESIZER_PLAYLISTNAVIGATOR_H
#define MIDI_PARSERSYNTHESIZER_PLAYLISTNAVIGATOR_H
#include <stack>
#include <string>
#include <vector>

enum class NavigationDirection {
    FORWARD,
    BACKWARD,
    JUMP_TO
};

class PlaylistNavigator {
private:
    std::vector<std::string> loaded_files;

    // Tracking logic
    std::stack<std::size_t> history_stack;
    std::stack<std::size_t> future_stack;
    std::size_t current_idx;
    bool has_active_track;

    bool shuffle_flag;
    bool repeat_flag;

    bool is_file_loaded(const std::string& file_path);

public:
    explicit PlaylistNavigator(const std::vector<std::string>& loaded_files = std::vector<std::string>());

    // Playback controls
    [[nodiscard]] std::size_t get_next_idx(bool forced_skip = false) const;
    [[nodiscard]] std::size_t get_previous_idx() const;
    bool commit_to_history(std::size_t new_idx, NavigationDirection direction);

    // Other manipulators
    void add_to_loaded_files(const std::string& file_path);
    void set_shuffle(bool enabled);
    void set_repeat(bool enabled);

    // Accessors for GUI updates
    [[nodiscard]] bool get_shuffle() const;
    [[nodiscard]] bool get_repeat() const;
};

#endif //MIDI_PARSERSYNTHESIZER_PLAYLISTNAVIGATOR_H