#ifndef MIDI_PARSERSYNTHESIZER_MAINWINDOW_H
#define MIDI_PARSERSYNTHESIZER_MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>
#include <QLabel>

class PlaybackController;
class QHBoxLayout;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // Manages the coordination between AudioEngine backend and Qt UI front-end
    PlaybackController* playback_controller;

    // UI element: play/pause
    QIcon play_icon;
    QIcon pause_icon;
    QPushButton* play_pause_button;

    // UI element: next track sequence
    QIcon skip_fwd_icon;
    QPushButton* skip_fwd_button;

    // UI element: previous track sequence
    QIcon skip_back_icon;
    QPushButton* skip_back_button;

    // UI element: repeat toggle
    QIcon repeat_off_icon;
    QIcon repeat_on_icon;
    QPushButton* repeat_button;

    // UI element: shuffle toggle
    QIcon shuffle_off_icon;
    QIcon shuffle_on_icon;
    QPushButton* shuffle_button;

    // UI element: volume icon
    QIcon volume_mute_icon;
    QIcon volume_low_icon;
    QIcon volume_max_icon;
    QLabel* volume_label;

    // UI element: underrun warning
    QLabel* underrun_label;

    // UI element: file menu in top bar
    QMenu* file_menu;
    QAction* add_file_action;
    QAction* add_directory_action;
    QAction* exit_action;

    // UI element: playback menu in top bar
    QMenu* playback_menu;
    QAction* toggle_peak_amplitude_action;
    QAction* toggle_autoplay_action;

    // UI element: dropdown box for track selection
    QComboBox* track_selector;

    // UI element: timer display
    QLabel* track_sequence_length_label;

    // UI element: volume slider
    QSlider* volume_slider;

    // UI element: playback (seek) slider
    QSlider* seek_slider;

    void init_top_bar();
    void init_top_ui(QHBoxLayout* layout);
    void init_middle_ui(QHBoxLayout* layout);
    void init_bottom_ui(QHBoxLayout* layout);
    void init_status_bar();
    void init_actions();
    void init_connections();

    // Prevent direct coupling of PlaybackController to QString
    void on_add_directory_button_clicked();
    void on_add_file_button_clicked();

private slots:
    void on_playback_state_changed(bool is_playing);
    void on_repeat_changed(bool repeat_flag);
    void on_shuffle_changed(bool shuffle_flag);
    void on_autoplay_changed(bool autoplay_flag);
    void on_track_list_updated(const std::vector<std::string>& file_paths);
    void on_current_track_changed(std::size_t index);
    void on_volume_changed(int volume);
    void on_time_updated(float current_seconds, float total_seconds);
    void on_underrun_detected(bool status);
    void on_peak_amplitude_normalization_changed(bool enabled);

public:
    explicit MainWindow(PlaybackController* controller, QWidget* parent = nullptr);
};


#endif //MIDI_PARSERSYNTHESIZER_MAINWINDOW_H