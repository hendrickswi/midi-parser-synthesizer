#include "MainWindow.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSizePolicy>
#include "../AudioEngine/AudioEngine.h"
#include "../DirectoryManipulator.h"

void MainWindow::init_top_ui(QHBoxLayout* layout) {
    // File select dropdown
    track_selector = new QComboBox(this);
    for (const auto& name : engine->get_loaded_file_names()) {
        track_selector->addItem(QString::fromStdString(name));
    }

    connect(track_selector, &QComboBox::currentIndexChanged, this, &MainWindow::on_track_selection_changed);
    layout->addWidget(track_selector);

    // Add directory button
    add_directory_button = new QPushButton("Add Directory", this);
    add_directory_button->setFixedWidth(150);
    connect(add_directory_button, &QPushButton::clicked, this, &MainWindow::on_add_directory_button_clicked);
    layout->addWidget(add_directory_button);
}

void MainWindow::init_middle_ui(QHBoxLayout* layout) {
    // Creating the time display
    track_sequence_length_label = new QLabel("0.00 / 0.00 s", this);
    track_sequence_length_label->setAlignment(Qt::AlignCenter);
    track_sequence_length_label->setMinimumWidth(120);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update_timer);

    // Creating the volume slider
    volume_slider = new QSlider(Qt::Vertical, this);
    volume_slider->setRange(0, 100);
    volume_slider->setValue(50);
    volume_slider->setFixedHeight(100);
    connect(volume_slider, &QAbstractSlider::valueChanged, this, &MainWindow::on_volume_slider_value_changed);

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
    connect(seek_slider, &QAbstractSlider::sliderMoved, this, &MainWindow::on_seek_slider_moved);

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
    connect(repeat_button, &QPushButton::clicked, this, &MainWindow::on_repeat_button_clicked);

    // Shuffle button
    shuffle_off_icon = QIcon(":/Assets/img/shuffle.png");
    shuffle_on_icon = QIcon(":/Assets/img/shuffle_on.png");
    shuffle_button = new QPushButton(shuffle_off_icon, "", this);
    shuffle_button->setIconSize(icon_size);
    shuffle_button->setStyleSheet(transparent_button_style);
    shuffle_button->setCursor(Qt::PointingHandCursor);
    connect(shuffle_button, &QPushButton::clicked, this, &MainWindow::on_shuffle_button_clicked);

    // Autoplay button
    autoplay_off_icon = QIcon(":/Assets/img/autoplay.png");
    autoplay_button = new QPushButton(autoplay_off_icon, "", this);
    autoplay_button->setIconSize(icon_size);
    autoplay_button->setStyleSheet(transparent_button_style);
    autoplay_button->setCursor(Qt::PointingHandCursor);
    connect(autoplay_button, &QPushButton::clicked, this, &MainWindow::on_autoplay_button_clicked);

    // Previous track sequence button
    skip_back_icon = QIcon(":/Assets/img/skip_back.png");
    skip_back_button = new QPushButton(skip_back_icon, "", this);
    skip_back_button->setIconSize(icon_size);
    skip_back_button->setStyleSheet(transparent_button_style);
    skip_back_button->setCursor(Qt::PointingHandCursor);
    connect(skip_back_button, &QPushButton::clicked, this, &MainWindow::on_skip_back_button_clicked);

    // Play and pause button
    play_icon = QIcon(":/Assets/img/play.png");
    pause_icon = QIcon(":/Assets/img/pause.png");
    play_pause_button = new QPushButton(play_icon, "", this);
    play_pause_button->setIconSize(icon_size);
    play_pause_button->setStyleSheet(transparent_button_style);
    play_pause_button->setCursor(Qt::PointingHandCursor);
    connect(play_pause_button, &QPushButton::clicked, this, &MainWindow::on_play_pause_button_clicked);

    // Next track sequence button
    skip_fwd_icon = QIcon(":/Assets/img/skip_fwd.png");
    skip_fwd_button = new QPushButton(skip_fwd_icon, "", this);
    skip_fwd_button->setIconSize(icon_size);
    skip_fwd_button->setStyleSheet(transparent_button_style);
    skip_fwd_button->setCursor(Qt::PointingHandCursor);
    connect(skip_fwd_button, &QPushButton::clicked, this, &MainWindow::on_skip_fwd_button_clicked);


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

void MainWindow::on_song_end() {
    engine->stop();
    timer->stop();
    play_pause_button->setIcon(play_icon);
}

void MainWindow::on_song_start() {
    engine->play();
    timer->start(33);
    play_pause_button->setIcon(pause_icon);
}

void MainWindow::on_song_unique_start() {
    on_song_start();
    play_history.push_back(track_selector->currentIndex());
}

MainWindow::MainWindow(AudioEngine* engine, QWidget *parent)
    : QMainWindow(parent), engine(engine) {
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

    setCentralWidget(central_widget);

    if (track_selector->count() > 0) {
        engine->set_track_sequence(0);
        update_timer();
    }

    repeat_flag = false;
    shuffle_flag = false;
    autoplay_flag = true;
}

MainWindow::~MainWindow() {
    timer->stop();
    // Destructor logic for any QObject* instantiated in MainWindow::MainWindow not needed here
}

void MainWindow::on_play_pause_button_clicked() {
    if (engine->is_playing()) {
        on_song_end();
    }
    else {
        if (!play_history.empty() && play_history.back() == track_selector->currentIndex()) {
            on_song_start();
        }
        else {
            on_song_unique_start();
        }
    }
}

void MainWindow::on_skip_fwd_button_clicked() {
    engine->stop();
    engine->soft_reset();

    if (shuffle_flag) {
        std::size_t random_idx = std::rand() % track_selector->count();
        engine->set_track_sequence(random_idx);
        track_selector->setCurrentIndex(random_idx);
    }
    else {
        std::size_t next_idx = (track_selector->currentIndex() + 1) % track_selector->count();
        engine->set_track_sequence(next_idx);
        track_selector->setCurrentIndex(next_idx);
    }

    on_song_unique_start();
    update_timer();
}

void MainWindow::on_skip_back_button_clicked() {
    std::size_t prev_idx = 0;
    if (!play_history.empty()) {
        prev_idx = play_history.back();
        play_history.pop_back();
    }

    track_selector->setCurrentIndex(prev_idx);
    engine->stop();
    engine->soft_reset();
    engine->set_track_sequence(prev_idx);
    on_song_start();
    update_timer();
}

void MainWindow::on_add_directory_button_clicked() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Directory");
    if (directory.isEmpty()) return;

    bool was_empty = track_selector->count() == 0;
    bool added_new_files = false;

    DirectoryManipulator directory_manipulator = DirectoryManipulator(directory.toStdString());
    auto midi_files = directory_manipulator.get_midi_files_in_directory();
    for (const auto& file : midi_files) {
        std::string file_path = file.get_file_path();
        if (engine->load_midi_file(file_path)) {
            track_selector->addItem(QString::fromStdString(file_path));
            added_new_files = true;
        }
    }

    // Automatically update dropdown if went from 0 to ( > 0) amount of files.
    if (was_empty && added_new_files) {
        track_selector->setCurrentIndex(0);
        engine->set_track_sequence(0);
        update_timer();
    }
}

void MainWindow::on_track_selection_changed(int index) {
    if (index < 0 || index >= engine->get_loaded_file_names().size()) return;
    engine->stop();
    engine->set_track_sequence(index);
    update_timer();
}

void MainWindow::on_volume_slider_value_changed(int volume) {
    engine->set_global_volume(volume / 100.0f);
    if (volume == 0) {
        volume_label->setPixmap(volume_mute_icon.pixmap(20, 20));
    }
    else if (volume < 50) {
        volume_label->setPixmap(volume_low_icon.pixmap(20, 20));
    }
    else {
        volume_label->setPixmap(volume_max_icon.pixmap(20, 20));
    }
}

void MainWindow::on_repeat_button_clicked() {
    repeat_flag = !repeat_flag;
    if (repeat_flag) {
        repeat_button->setIcon(repeat_on_icon);
    }
    else {
        repeat_button->setIcon(repeat_off_icon);
    }
}

void MainWindow::on_shuffle_button_clicked() {
    shuffle_flag = !shuffle_flag;
    if (shuffle_flag) {
        shuffle_button->setIcon(shuffle_on_icon);
    }
    else {
        shuffle_button->setIcon(shuffle_off_icon);
    }
}

void MainWindow::on_autoplay_button_clicked() {
    autoplay_flag = !autoplay_flag;
    if (autoplay_flag) {
        autoplay_button->setIcon(autoplay_on_icon);
    }
    else {
        autoplay_button->setIcon(autoplay_off_icon);
    }
}

void MainWindow::on_seek_slider_moved(int pos) {
    float total_seconds = engine->get_track_sequence_length_seconds();
    float new_time = static_cast<float>(pos) / 1000.0f * total_seconds;
    engine->skip_seconds(new_time - engine->get_track_sequence_current_time_seconds());
    update_timer();
}

void MainWindow::update_timer() {
    float current = engine->get_track_sequence_current_time_seconds();
    float total = engine->get_track_sequence_length_seconds();

    // Timer display update logic
    QString time_str = QString::asprintf("%.2f / %.2f s", current, total);
    track_sequence_length_label->setText(time_str);

    // Seek "playback" slider update logic
    if (total > 0.0f) {
        seek_slider->blockSignals(true);
        int slider_val = static_cast<int>((current / total) * 1000.0f);
        seek_slider->setValue(slider_val);
        seek_slider->blockSignals(false);
    }

    // Case: the song has ended
    if (!engine->is_playing() && timer->isActive()) {
        on_song_end();
    }
}