#ifndef MIDI_PARSERSYNTHESIZER_PLAYLISTNAVIGATOR_H
#define MIDI_PARSERSYNTHESIZER_PLAYLISTNAVIGATOR_H
#include <string>
#include <vector>

class PlaylistNavigator {
private:
    std::vector<std::string> loaded_files;
    std::vector<std::size_t> playback_history;

    bool shuffle_flag;
    bool repeat_flag;

    bool is_file_loaded(const std::string& file_path);

public:
    explicit PlaylistNavigator(const std::vector<std::string>& loaded_files = std::vector<std::string>());

    // Playback controls
    std::size_t get_next_idx(bool forced_skip = false);
    std::size_t get_previous_idx();
    bool commit_to_history(std::size_t idx);

    // Other manipulators
    void add_to_loaded_files(const std::string& file_path);
    void set_shuffle(bool enabled);
    void set_repeat(bool enabled);

    // Accessors for GUI updates
    [[nodiscard]] bool get_shuffle() const;
    [[nodiscard]] bool get_repeat() const;
};

#endif //MIDI_PARSERSYNTHESIZER_PLAYLISTNAVIGATOR_H