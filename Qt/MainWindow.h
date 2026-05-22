#ifndef MIDI_PARSERSYNTHESIZER_MAINWINDOW_H
#define MIDI_PARSERSYNTHESIZER_MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>
#include <QLabel>

class QHBoxLayout;
class AudioEngine;

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    AudioEngine* engine;

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

    // UI element: autoplay toggle
    QIcon autoplay_off_icon;
    QIcon autoplay_on_icon;
    QPushButton* autoplay_button;

    // UI element: add directory
    QIcon add_directory_icon;
    QPushButton* add_directory_button;

    // UI element: volume icon
    QIcon volume_mute_icon;
    QIcon volume_low_icon;
    QIcon volume_max_icon;
    QLabel* volume_label;

    QComboBox* track_selector;
    QLabel* track_sequence_length_label;
    QTimer* timer;
    QSlider* volume_slider;
    QSlider* seek_slider;

    bool repeat_flag;
    bool shuffle_flag;
    bool autoplay_flag;

    std::vector<std::size_t> play_history;

    void init_top_ui(QHBoxLayout* layout);
    void init_middle_ui(QHBoxLayout* layout);
    void init_bottom_ui(QHBoxLayout* layout);

    // Internal delegates
    void on_song_end();
    void on_song_start();
    void on_song_unique_start();

private slots:
    void on_play_pause_button_clicked();
    void on_skip_fwd_button_clicked();
    void on_skip_back_button_clicked();
    void on_repeat_button_clicked();
    void on_shuffle_button_clicked();
    void on_autoplay_button_clicked();
    void on_add_directory_button_clicked();
    void on_track_selection_changed(int index);
    void on_volume_slider_value_changed(int volume);
    void on_seek_slider_moved(int pos);

    void update_timer();

public:
    explicit MainWindow(AudioEngine* engine, QWidget* parent = nullptr);
    ~MainWindow();
};


#endif //MIDI_PARSERSYNTHESIZER_MAINWINDOW_H