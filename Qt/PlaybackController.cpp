#include "PlaybackController.h"
#include "../DirectoryManipulator.h"
#include "../AudioEngine/AudioEngine.h"

void PlaybackController::on_track_sequence_change(std::size_t idx, NavigationDirection skip_direction, bool start_automatically, bool update_navigator) {
    if (idx >= engine->get_loaded_file_names().size() && idx == engine->get_current_track_sequence_index()) return;

    set_state(PlaybackState::STOPPED);
    engine->set_track_sequence(idx);
    current_track_changed(idx);
    time_updated(engine->get_track_sequence_current_time_seconds(), engine->get_track_sequence_length_seconds());

    if (start_automatically) {
        set_state(PlaybackState::PLAYING);
        if (update_navigator) {
            navigator.commit_to_history(idx, skip_direction);
        }
    }

    // Info printout
    auto instrument_names = engine->get_instrument_names_of_current_track_sequence();
    std::cout << "INFO: Selected track contains instruments:" << std::endl;
    for (const auto& name : instrument_names) {
        std::cout << name << std::endl;
    }
    std::cout << std::endl;
}

void PlaybackController::set_state(PlaybackState new_state) {
    if (new_state == current_state) return;

    // Remove old state
    switch (current_state) {
        case PlaybackState::PLAYING : {
            playback_timer->stop();
            underrun_timer->stop();
            engine->stop();
            break;
        }
        default: {
            break;
        }
    }

    // Set up new state
    current_state = new_state;
    switch (current_state) {
        case PlaybackState::PLAYING : {
            engine->play();
            playback_timer->start(33);
            underrun_timer->start(500);
            playback_state_changed(true);
            break;
        }
        case PlaybackState::PAUSED : {
            engine->stop();
            playback_timer->stop();
            underrun_timer->stop();
            playback_state_changed(false);
            break;
        }
        case PlaybackState::STOPPED : {
            engine->stop();
            playback_timer->stop();
            underrun_timer->stop();
            playback_state_changed(false);
            engine->soft_reset();
            break;
        }
    }

}

PlaybackController::PlaybackController(AudioEngine* engine, QObject* parent)
    : QObject(parent), engine(engine) {
    playback_timer = new QTimer(this);
    underrun_timer = new QTimer(this);
    prev_underrun_count = 0;
    underrun_warning_ticks = 0;
    current_state = PlaybackState::PLAYING; // Ensure initialization logic
    set_state(PlaybackState::STOPPED);
    navigator = PlaylistNavigator();
    autoplay_enabled = true;
    first_time = true;

    connect(playback_timer, &QTimer::timeout, this, &PlaybackController::on_playback_timer_tick);
    connect(underrun_timer, &QTimer::timeout, this, &PlaybackController::on_underrun_timer_tick);

    // Initialize first track sequence

}

void PlaybackController::toggle_play_pause() {
    if (engine->is_playing()) {
        // User pressed the pause button
        set_state(PlaybackState::PAUSED);
    }
    else {
        if (first_time) {
            on_track_sequence_change(0, NavigationDirection::JUMP_TO, autoplay_enabled, true);
            first_time = false;
        } else {
            set_state(PlaybackState::PLAYING);
        }
    }
}

void PlaybackController::skip_forward() {
    on_track_sequence_change(navigator.get_next_idx(true), NavigationDirection::FORWARD, autoplay_enabled, true);
}

void PlaybackController::skip_backward() {
    on_track_sequence_change(navigator.get_previous_idx(), NavigationDirection::BACKWARD, autoplay_enabled, true);
}

void PlaybackController::toggle_repeat() {
    navigator.set_repeat(!navigator.get_repeat());
    repeat_changed(navigator.get_repeat());
}

void PlaybackController::toggle_shuffle() {
    navigator.set_shuffle(!navigator.get_shuffle());
    shuffle_changed(navigator.get_shuffle());
}

void PlaybackController::toggle_autoplay() {
    autoplay_enabled = !autoplay_enabled;
    autoplay_changed(autoplay_enabled);
}

void PlaybackController::toggle_peak_amplitude_normalization() {
    bool new_status = !engine->get_peak_amplitude_normalization();
    engine->set_peak_amplitude_normalization(new_status);
    peak_amplitude_normalization_changed(new_status);
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
            navigator.add_to_loaded_files(file_path);
            updated_files.push_back(file_path);
            added_new_files = true;
        }
    }

    // Automatically update if we went from 0 to ( > 0) amount of files.
    if (added_new_files) {
        track_list_updated(updated_files);
        if (was_empty) {
            first_loaded();
        }
    }
}

void PlaybackController::load_file(const std::string& file_path) {
    if (file_path.empty()) return;

    std::vector<std::string> updated_files = std::vector<std::string>();
    const std::vector<std::string>& prev_loaded_file_names = engine->get_loaded_file_names();
    for (const auto& file : prev_loaded_file_names) {
        updated_files.push_back(file);
    }

    bool was_empty = updated_files.size() == 0;
    bool added_new_files = false;

    if (engine->load_midi_file(file_path)) {
        navigator.add_to_loaded_files(file_path);
        updated_files.push_back(file_path);
        added_new_files = true;
    }

    // Automatically update if we went from 0 to ( > 0) amount of files.
    if (added_new_files) {
        track_list_updated(updated_files);
        if (was_empty) {
            first_loaded();
        }
    }
}

void PlaybackController::select_track(std::size_t idx) {
    if (idx >= engine->get_loaded_file_names().size()) return;
    on_track_sequence_change(idx, NavigationDirection::JUMP_TO, autoplay_enabled, true);
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

void PlaybackController::on_playback_timer_tick() {
    time_updated(engine->get_track_sequence_current_time_seconds(), engine->get_track_sequence_length_seconds());
    if (!engine->is_playing()) {
        if (autoplay_enabled && !engine->get_loaded_file_names().empty()) {
            on_track_sequence_change(navigator.get_next_idx(), NavigationDirection::FORWARD, true, true);
        } else {
            set_state(PlaybackState::STOPPED);
        }
    }
}

void PlaybackController::on_underrun_timer_tick() {
    auto current_underrun_count = engine->get_underrun_count();

    if (current_underrun_count > prev_underrun_count) {
        underrun_detected(true);
        underrun_warning_ticks = 4;
    }
    else {
        if (underrun_warning_ticks > 0) {
            underrun_warning_ticks--;
        } else {
            underrun_detected(false);
        }
    }

    prev_underrun_count = current_underrun_count;
}