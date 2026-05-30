#include "MainWindow.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSizePolicy>

#include "PlaybackController.h"
#include "../AudioEngine/AudioEngine.h"
#include "../DirectoryManipulator.h"

void MainWindow::init_top_ui(QHBoxLayout* layout) {
    // File select dropdown
    track_selector = new QComboBox(this);

    // Add directory button
    add_directory_button = new QPushButton("Add Directory", this);
    add_directory_button->setFixedWidth(150);

    // Add file button
    add_file_button = new QPushButton("Add File", this);

    layout->addWidget(track_selector);
    layout->addWidget(add_directory_button);
    layout->addWidget(add_file_button);
}

void MainWindow::init_middle_ui(QHBoxLayout* layout) {
    // Creating the time display
    track_sequence_length_label = new QLabel("0.00 / 0.00 s", this);
    track_sequence_length_label->setAlignment(Qt::AlignCenter);
    track_sequence_length_label->setMinimumWidth(120);

    // Creating the volume slider
    volume_slider = new QSlider(Qt::Vertical, this);
    volume_slider->setRange(0, 200);
    volume_slider->setValue(100);
    volume_slider->setFixedHeight(100);

    // Creating the volume icon
    volume_mute_icon = QIcon(":/Assets/img/volume_mute.png");
    volume_low_icon = QIcon(":/Assets/img/volume_low.png");
    volume_max_icon = QIcon(":/Assets/img/volume_max.png");
    volume_label = new QLabel(this);
    volume_label->setPixmap(volume_low_icon.pixmap(20, 20));

    // Creating the seek (playback) slider
    seek_slider = new QSlider(Qt::Horizontal, this);
    seek_slider->setRange(0, 1000);
    seek_slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout->addSpacing(20);
    layout->addWidget(volume_label);
    layout->addSpacing(5);
    layout->addWidget(volume_slider);
    layout->addSpacing(20);
    layout->addWidget(track_sequence_length_label);
    layout->addWidget(seek_slider);
    layout->addSpacing(20);
}

void MainWindow::init_bottom_ui(QHBoxLayout* layout) {
    QSize icon_size(40, 40);
    QString transparent_button_style =
        "QPushButton { background-color: transparent; border: none; outline: none; }"
        "QPushButton:hover { background: rgba(128, 128, 128, 0.2); border-radius:5px; }";

    // Repeat button
    repeat_off_icon = QIcon(":/Assets/img/repeat.png");
    repeat_on_icon = QIcon(":/Assets/img/repeat_on.png");
    repeat_button = new QPushButton(repeat_off_icon,"", this);
    repeat_button->setIconSize(icon_size);
    repeat_button->setStyleSheet(transparent_button_style);
    repeat_button->setCursor(Qt::PointingHandCursor);

    // Shuffle button
    shuffle_off_icon = QIcon(":/Assets/img/shuffle.png");
    shuffle_on_icon = QIcon(":/Assets/img/shuffle_on.png");
    shuffle_button = new QPushButton(shuffle_off_icon, "", this);
    shuffle_button->setIconSize(icon_size);
    shuffle_button->setStyleSheet(transparent_button_style);
    shuffle_button->setCursor(Qt::PointingHandCursor);

    // Autoplay button
    autoplay_off_icon = QIcon(":/Assets/img/autoplay.png");
    autoplay_on_icon = QIcon(":/Assets/img/autoplay_on.png");
    autoplay_button = new QPushButton(autoplay_on_icon, "", this);
    autoplay_button->setIconSize(icon_size);
    autoplay_button->setStyleSheet(transparent_button_style);
    autoplay_button->setCursor(Qt::PointingHandCursor);

    // Previous track sequence button
    skip_back_icon = QIcon(":/Assets/img/skip_back.png");
    skip_back_button = new QPushButton(skip_back_icon, "", this);
    skip_back_button->setIconSize(icon_size);
    skip_back_button->setStyleSheet(transparent_button_style);
    skip_back_button->setCursor(Qt::PointingHandCursor);

    // Play and pause button
    play_icon = QIcon(":/Assets/img/play.png");
    pause_icon = QIcon(":/Assets/img/pause.png");
    play_pause_button = new QPushButton(play_icon, "", this);
    play_pause_button->setIconSize(icon_size);
    play_pause_button->setStyleSheet(transparent_button_style);
    play_pause_button->setCursor(Qt::PointingHandCursor);

    // Next track sequence button
    skip_fwd_icon = QIcon(":/Assets/img/skip_fwd.png");
    skip_fwd_button = new QPushButton(skip_fwd_icon, "", this);
    skip_fwd_button->setIconSize(icon_size);
    skip_fwd_button->setStyleSheet(transparent_button_style);
    skip_fwd_button->setCursor(Qt::PointingHandCursor);

    layout->addSpacing(20);
    layout->addWidget(repeat_button);
    layout->addWidget(shuffle_button);
    layout->addStretch();
    layout->addWidget(skip_back_button);
    layout->addWidget(play_pause_button);
    layout->addWidget(skip_fwd_button);
    layout->addStretch();
    layout->addWidget(autoplay_button);
    layout->addSpacing(20);
}

void MainWindow::init_connections() {
    /*
     * Each widget needs a two-way connection
     * (view to controller, and controller to view)
     *
     * Additionally, some signals may be triggered by multiple sources
     */

    // Track selector
    connect(track_selector, &QComboBox::currentIndexChanged, playback_controller, &PlaybackController::select_track);
    connect(playback_controller, &PlaybackController::current_track_changed, this, &MainWindow::on_current_track_changed);
    connect(playback_controller, &PlaybackController::first_loaded, this, &MainWindow::on_current_track_changed);

    // Add directory button
    connect(add_directory_button, &QPushButton::clicked, this, &MainWindow::on_add_directory_button_clicked);
    connect(playback_controller, &PlaybackController::track_list_updated, this, &MainWindow::on_track_list_updated);

    // Add file button
    connect(add_file_button, &QPushButton::clicked, this, &MainWindow::on_add_file_button_clicked);
    connect(playback_controller, &PlaybackController::track_list_updated, this, &MainWindow::on_track_list_updated);

    // Volume slider
    connect(volume_slider, &QAbstractSlider::valueChanged, playback_controller, &PlaybackController::set_volume);
    connect(playback_controller, &PlaybackController::volume_changed, this, &MainWindow::on_volume_changed);

    // Seek slider
    connect(seek_slider, &QAbstractSlider::sliderMoved, playback_controller, &PlaybackController::seek_to);
    connect(playback_controller, &PlaybackController::time_updated, this, &MainWindow::on_time_updated);

    // Repeat button
    connect(repeat_button, &QPushButton::clicked, playback_controller, &PlaybackController::toggle_repeat);
    connect(playback_controller, &PlaybackController::repeat_changed, this, &MainWindow::on_repeat_changed);

    // Shuffle button
    connect(shuffle_button, &QPushButton::clicked, playback_controller, &PlaybackController::toggle_shuffle);
    connect(playback_controller, &PlaybackController::shuffle_changed, this, &MainWindow::on_shuffle_changed);

    // Autoplay button
    connect(autoplay_button, &QPushButton::clicked, playback_controller, &PlaybackController::toggle_autoplay);
    connect(playback_controller, &PlaybackController::autoplay_changed, this, &MainWindow::on_autoplay_changed);

    // Previous and next track sequence buttons
    connect(skip_back_button, &QPushButton::clicked, playback_controller, &PlaybackController::skip_backward);
    connect(skip_fwd_button, &QPushButton::clicked, playback_controller, &PlaybackController::skip_forward);
    connect(playback_controller, &PlaybackController::current_track_changed, this, &MainWindow::on_current_track_changed);

    // Play and pause button
    connect(play_pause_button, &QPushButton::clicked, playback_controller, &PlaybackController::toggle_play_pause);
    connect(playback_controller, &PlaybackController::playback_state_changed, this, &MainWindow::on_playback_state_changed);
}

void MainWindow::on_add_directory_button_clicked() {
    QString directory_path = QFileDialog::getExistingDirectory(this, "Select Directory");
    playback_controller->load_directory(directory_path.toStdString());
}

void MainWindow::on_add_file_button_clicked() {
    QString file_path = QFileDialog::getOpenFileName(this, "Select a file");
    playback_controller->load_file(file_path.toStdString());
}

MainWindow::MainWindow(PlaybackController* playback_controller, QWidget *parent)
    : QMainWindow(parent), playback_controller(playback_controller) {
    setWindowTitle("MIDI Synthesizer");
    setMinimumSize(400, 300);
    resize(600, 300);

    QWidget* central_widget = new QWidget(this);
    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);

    QHBoxLayout* top_layout = new QHBoxLayout();
    init_top_ui(top_layout);
    main_layout->addLayout(top_layout);

    QHBoxLayout* middle_layout = new QHBoxLayout();
    init_middle_ui(middle_layout);
    main_layout->addLayout(middle_layout);

    QHBoxLayout* bottom_layout = new QHBoxLayout();
    init_bottom_ui(bottom_layout);
    main_layout->addLayout(bottom_layout);

    init_connections();
    setCentralWidget(central_widget);
}

void MainWindow::on_playback_state_changed(bool is_playing) {
    if (is_playing) {
        play_pause_button->setIcon(pause_icon);
    } else {
        play_pause_button->setIcon(play_icon);
    }
}

void MainWindow::on_repeat_changed(bool repeat_flag) {
    if (repeat_flag) {
        repeat_button->setIcon(repeat_on_icon);
    } else {
        repeat_button->setIcon(repeat_off_icon);
    }
}

void MainWindow::on_shuffle_changed(bool shuffle_flag) {
    if (shuffle_flag) {
        shuffle_button->setIcon(shuffle_on_icon);
    } else {
        shuffle_button->setIcon(shuffle_off_icon);
    }
}

void MainWindow::on_autoplay_changed(bool autoplay_flag) {
    if (autoplay_flag) {
        autoplay_button->setIcon(autoplay_on_icon);
    } else {
        autoplay_button->setIcon(autoplay_off_icon);
    }
}

void MainWindow::on_track_list_updated(const std::vector<std::string>& file_paths) {
    track_selector->blockSignals(true);
    track_selector->clear();
    for (const auto& path : file_paths) {
        track_selector->addItem(QString::fromStdString(path));
    }
    track_selector->blockSignals(false);
}

void MainWindow::on_current_track_changed(std::size_t index) {
    track_selector->blockSignals(true);
    track_selector->setCurrentIndex(index);
    track_selector->blockSignals(false);
}

void MainWindow::on_volume_changed(int volume) {
    if (volume == 0) {
        volume_label->setPixmap(volume_mute_icon.pixmap(20, 20));
    }
    else if (volume < 100) {
        volume_label->setPixmap(volume_low_icon.pixmap(20, 20));
    }
    else {
        volume_label->setPixmap(volume_max_icon.pixmap(20, 20));
    }

    volume_slider->blockSignals(true);
    volume_slider->setValue(volume);
    volume_slider->blockSignals(false);
}

void MainWindow::on_time_updated(float current_seconds, float total_seconds) {
    // Timer display update logic
    QString time_str = QString::asprintf("%.1f / %.1f s", current_seconds, total_seconds);
    track_sequence_length_label->setText(time_str);

    // Seek "playback" slider update logic
    if (total_seconds > 0.0f) {
        seek_slider->blockSignals(true);
        int slider_val = static_cast<int>(current_seconds / total_seconds * 1000.0f);
        seek_slider->setValue(slider_val);
        seek_slider->blockSignals(false);
    }
}