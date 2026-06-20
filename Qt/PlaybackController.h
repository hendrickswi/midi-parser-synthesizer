#ifndef MIDI_PARSERSYNTHESIZER_PLAYBACKCONTROLLER_H
#define MIDI_PARSERSYNTHESIZER_PLAYBACKCONTROLLER_H
#include <QTimer>
#include "../AudioEngine/PlaylistNavigator.h"

constexpr float DELAY_BETWEEN_TRACK_SEQUENCES_SECONDS = 1.0f;

enum class PlaybackState {
    STOPPED,
    PAUSED,
    PLAYING,
    TRANSITIONING
};

class AudioEngine;

class PlaybackController : public QObject {
    Q_OBJECT

private:
    AudioEngine* engine;
    QTimer* playback_timer;
    QTimer* underrun_timer;
    QTimer* delay_timer;
    uint64_t prev_underrun_count;
    unsigned int underrun_warning_ticks;

    // State machine
    PlaybackState current_state;

    // Next/prev song logic
    PlaylistNavigator navigator;
    bool autoplay_enabled;
    bool first_time;

    // Internal delegates
    void on_track_sequence_change(std::size_t idx, NavigationDirection skip_direction, bool start_automatically, bool update_navigator);
    void set_state(PlaybackState new_state);

public:
    explicit PlaybackController(AudioEngine* engine, QObject* parent = nullptr);

signals:
    // Controller to view
    void playback_state_changed(bool is_playing);
    void repeat_changed(bool repeat_flag);
    void shuffle_changed(bool shuffle_flag);
    void autoplay_changed(bool autoplay_flag);
    void peak_amplitude_normalization_changed(bool enabled);
    void track_list_updated(const std::vector<std::string>& tracks);
    void current_track_changed(std::size_t idx);
    void volume_changed(int volume);
    void time_updated(float current_seconds, float total_seconds);
    void first_loaded(int track_selector_start_idx = 0);
    void underrun_detected(bool status);

public slots:
    // View to controller
    void toggle_play_pause();
    void skip_forward();
    void skip_backward();
    void toggle_repeat();
    void toggle_shuffle();
    void toggle_autoplay();
    void toggle_peak_amplitude_normalization();
    void load_directory(const std::string& directory_path);
    void load_file(const std::string& file_path);
    void select_track(std::size_t index);
    void set_volume(int volume);
    void seek_to(int pos);

private slots:
    void on_playback_timer_tick();
    void on_underrun_timer_tick();
    void on_delay_timer_tick();
};

#endif //MIDI_PARSERSYNTHESIZER_PLAYBACKCONTROLLER_H