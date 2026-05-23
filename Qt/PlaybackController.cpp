#include "PlaybackController.h"
#include "../DirectoryManipulator.h"
#include "../AudioEngine/AudioEngine.h"

void PlaybackController::on_song_end() {
    engine->stop();
    playback_timer->stop();

    if (autoplay_flag) {
        if (shuffle_flag) {
            std::size_t random_idx = std::rand() % engine->get_loaded_file_names().size();
            on_track_sequence_change(random_idx);

            if (!play_history.empty() && random_idx == play_history.back()) {
                on_song_start();
            }
            else {
                on_song_unique_start();
            }
        }
        else if (repeat_flag) {
            on_song_start();
        }
        else {
            std::size_t next_idx = 0;
            if (!play_history.empty()) {
                next_idx = (play_history.back() + 1) % engine->get_loaded_file_names().size();
            }
            on_track_sequence_change(next_idx);
            on_song_unique_start();
        }
    }
    else {
        playback_state_changed(false);
    }
}

void PlaybackController::on_song_pause() {
    engine->stop();
    playback_timer->stop();
    playback_state_changed(false);
}

void PlaybackController::on_song_start() {
    engine->play();
    playback_timer->start(33);
    playback_state_changed(true);
}

void PlaybackController::on_song_unique_start() {
    on_song_start();
    play_history.push_back(engine->get_current_track_sequence_index());
}

void PlaybackController::on_track_sequence_change(std::size_t index) {
    engine->stop();
    engine->soft_reset();
    engine->set_track_sequence(index);
    current_track_changed(index);
    time_updated(engine->get_track_sequence_current_time_seconds(), engine->get_track_sequence_length_seconds());
}

PlaybackController::PlaybackController(AudioEngine* engine, QObject* parent)
    : QObject(parent), engine(engine) {
    playback_timer = new QTimer(this);
    connect(playback_timer, &QTimer::timeout, this, &PlaybackController::on_timer_tick);

    repeat_flag = false;
    shuffle_flag = false;
    autoplay_flag = true;
    play_history = std::vector<std::size_t>();
}

void PlaybackController::toggle_play_pause() {
    if (engine->is_playing()) {
        // User pressed the pause button
        on_song_pause();
    }
    else {
        if (!play_history.empty() && play_history.back() == engine->get_current_track_sequence_index()) {
            on_song_start();
        }
        else {
            on_song_unique_start();
        }
    }
}

void PlaybackController::skip_forward() {
    std::size_t next_idx;
    if (shuffle_flag) {
        next_idx = std::rand() % engine->get_loaded_file_names().size();
    }
    else {
        std::size_t idx = play_history.empty() ? 0 : play_history.back();
        next_idx = (idx + 1) % engine->get_loaded_file_names().size();
    }
    on_track_sequence_change(next_idx);
}

void PlaybackController::skip_backward() {
    std::size_t prev_idx = 0;
    if (!play_history.empty()) {
        prev_idx = play_history.back();
        play_history.pop_back();
    }
    on_track_sequence_change(prev_idx);
}

void PlaybackController::toggle_repeat() {
    repeat_flag = !repeat_flag;
    repeat_changed(repeat_flag);
}

void PlaybackController::toggle_shuffle() {
    shuffle_flag = !shuffle_flag;
    shuffle_changed(shuffle_flag);
}

void PlaybackController::toggle_autoplay() {
    autoplay_flag = !autoplay_flag;
    autoplay_changed(autoplay_flag);
}

void PlaybackController::load_directory(const std::string& directory_path) {
    if (directory_path.empty()) return;

    std::vector<std::string> updated_files = std::vector<std::string>();
    const std::vector<std::string>& prev_loaded_file_names = engine->get_loaded_file_names();
    for (const auto& file : prev_loaded_file_names) {
        updated_files.push_back(file);
    }

    bool was_empty = engine->get_loaded_file_names().size() == 0;
    bool added_new_files = false;

    DirectoryManipulator directory_manipulator = DirectoryManipulator(directory_path);
    auto midi_files = directory_manipulator.get_midi_files_in_directory();
    for (const auto& file : midi_files) {
        std::string file_path = file.get_file_path();
        if (engine->load_midi_file(file_path)) {
            updated_files.push_back(file_path);
            added_new_files = true;
        }
    }

    // Automatically update if we went from 0 to ( > 0) amount of files.
    if (added_new_files) {
        track_list_updated(updated_files);
        if (was_empty) {
            on_track_sequence_change(0);
        }
    }
}

void PlaybackController::select_track(std::size_t index) {
    if (index >= engine->get_loaded_file_names().size()) return;
    on_track_sequence_change(index);
}

void PlaybackController::set_volume(int volume) {
    engine->set_global_volume(volume / 100.0f);
    volume_changed(volume);
}

void PlaybackController::seek_to(int pos) {
    float total_seconds = engine->get_track_sequence_length_seconds();
    float new_time = static_cast<float>(pos) / 1000.0f * total_seconds;
    engine->skip_seconds(new_time - engine->get_track_sequence_current_time_seconds());
    time_updated(engine->get_track_sequence_current_time_seconds(), engine->get_track_sequence_length_seconds());
}

void PlaybackController::on_timer_tick() {
    time_updated(engine->get_track_sequence_current_time_seconds(), engine->get_track_sequence_length_seconds());
    if (!engine->is_playing()) {
        on_song_end();
    }
}