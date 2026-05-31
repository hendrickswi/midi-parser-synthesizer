#ifndef MIDI_PARSERSYNTHESIZER_PLAYBACKCONTROLLER_H
#define MIDI_PARSERSYNTHESIZER_PLAYBACKCONTROLLER_H
#include <QString>
#include <QTimer>

class AudioEngine;

class PlaybackController : public QObject {
    Q_OBJECT

private:
    AudioEngine* engine;
    QTimer* playback_timer;
    QTimer* underrun_timer;
    uint64_t prev_underrun_count;
    unsigned int underrun_warning_ticks;

    // State logic
    bool repeat_flag;
    bool shuffle_flag;
    bool autoplay_flag;
    std::vector<std::size_t> play_history;

    // Internal delegates
    void on_song_end();
    void on_song_pause();
    void on_song_start();
    void on_song_unique_start();
    void on_track_sequence_change(std::size_t index);

public:
    explicit PlaybackController(AudioEngine* engine, QObject* parent = nullptr);

signals:
    // Controller to view
    void playback_state_changed(bool is_playing);
    void repeat_changed(bool repeat_flag);
    void shuffle_changed(bool shuffle_flag);
    void autoplay_changed(bool autoplay_flag);
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
    void load_directory(const std::string& directory_path);
    void load_file(const std::string& file_path);
    void select_track(std::size_t index);
    void set_volume(int volume);
    void seek_to(int pos);

private slots:
    void on_playback_timer_tick();
    void on_underrun_timer_tick();
};

#endif //MIDI_PARSERSYNTHESIZER_PLAYBACKCONTROLLER_H