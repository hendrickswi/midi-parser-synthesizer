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

    // UI elements
    QPushButton* play_button;
    QPushButton* stop_button;
    QPushButton* skip_fwd_button;
    QPushButton* skip_back_button;
    QPushButton* add_directory_button;
    QComboBox* track_selector;
    QLabel* track_sequence_length_label;
    QTimer* timer;
    QSlider* volume_slider;

    void init_top_ui(QHBoxLayout* layout);
    void init_middle_ui(QHBoxLayout* layout);
    void init_bottom_ui(QHBoxLayout* layout);

private slots:
    void on_play_button_clicked();
    void on_stop_button_clicked();
    void on_skip_fwd_button_clicked();
    void on_skip_back_button_clicked();
    void on_add_directory_button_clicked();
    void on_track_selection_changed(int index);
    void on_volume_slider_value_changed(float volume);

    void update_timer();

public:
    explicit MainWindow(AudioEngine* engine, QWidget* parent = nullptr);
    ~MainWindow();
};


#endif //MIDI_PARSERSYNTHESIZER_MAINWINDOW_H