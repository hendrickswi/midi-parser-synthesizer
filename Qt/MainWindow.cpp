#include "MainWindow.h"
#include <QHBoxLayout>
#include <QFileDialog>
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
    layout->addWidget(track_sequence_length_label);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update_timer);

    // Creating the volume slider
    volume_slider = new QSlider(Qt::Vertical, this);
    volume_slider->setRange(0, 100);
    volume_slider->setValue(50);
    volume_slider->setFixedWidth(100);
    volume_slider->setFixedHeight(100);
    connect(volume_slider, &QAbstractSlider::valueChanged, this, &MainWindow::on_volume_slider_value_changed);
    layout->addWidget(volume_slider);

    // Spacing so the timer stays centered
    layout->addSpacing(100);
}

void MainWindow::init_bottom_ui(QHBoxLayout* layout) {
    // Repeat button
    repeat_button = new QPushButton("Repeat: Off", this);
    connect(repeat_button, &QPushButton::clicked, this, &MainWindow::on_repeat_button_clicked);
    layout->addWidget(repeat_button);

    // Shuffle button
    shuffle_button = new QPushButton("Shuffle: Off", this);
    connect(shuffle_button, &QPushButton::clicked, this, &MainWindow::on_shuffle_button_clicked);
    layout->addWidget(shuffle_button);

    // Autoplay button
    autoplay_button = new QPushButton("Autoplay: Off", this);
    connect(autoplay_button, &QPushButton::clicked, this, &MainWindow::on_autoplay_button_clicked);
    layout->addWidget(autoplay_button);

    // Skip 5s back button
    skip_back_button = new QPushButton("<< 5s", this);
    connect(skip_back_button, &QPushButton::clicked, this, &MainWindow::on_skip_back_button_clicked);
    layout->addWidget(skip_back_button);

    // Play button
    play_button = new QPushButton(QIcon("../Assets/img/play-button.png"), "Play", this);
    connect(play_button, &QPushButton::clicked, this, &MainWindow::on_play_button_clicked);
    layout->addWidget(play_button);

    // Stop button
    stop_button = new QPushButton(QIcon("../Assets/img/stop-button.png"), "Stop", this);
    connect(stop_button, &QPushButton::clicked, this, &MainWindow::on_stop_button_clicked);
    layout->addWidget(stop_button);

    // Skip 5s forward button
    skip_fwd_button = new QPushButton("5s >>", this);
    connect(skip_fwd_button, &QPushButton::clicked, this, &MainWindow::on_skip_fwd_button_clicked);
    layout->addWidget(skip_fwd_button);
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
    autoplay_flag = false;
}

MainWindow::~MainWindow() {
    timer->stop();
    // Destructor logic for any QObject* instantiated in MainWindow::MainWindow not needed here
}

void MainWindow::on_play_button_clicked() {
    if (engine->is_playing()) return;
    engine->play();

    // Update every 33 ms (~30 per sec)
    timer->start(33);
}

void MainWindow::on_stop_button_clicked() {
    if (!engine->is_playing()) return;
    engine->stop();
    timer->stop();
}

void MainWindow::on_skip_fwd_button_clicked() {
    engine->skip_seconds(5.0f);
    update_timer();
}

void MainWindow::on_skip_back_button_clicked() {
    engine->skip_seconds(-5.0f);
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

void MainWindow::on_volume_slider_value_changed(float volume) {
    engine->set_global_volume(volume / 100.0f);
}

void MainWindow::on_repeat_button_clicked() {
    repeat_flag = !repeat_flag;
    QString str = QString::fromStdString(repeat_flag ? "Repeat: On" : "Repeat: Off");
    repeat_button->setText(str);
}

void MainWindow::on_shuffle_button_clicked() {
    shuffle_flag = !shuffle_flag;
    QString str = QString::fromStdString(shuffle_flag ? "Shuffle: On" : "Shuffle: Off");
    shuffle_button->setText(str);
}

void MainWindow::on_autoplay_button_clicked() {
    autoplay_flag = !autoplay_flag;
    QString str = QString::fromStdString(autoplay_flag ? "Autoplay: On" : "Autoplay: Off");
    autoplay_button->setText(str);
}

void MainWindow::update_timer() {
    float current = engine->get_track_sequence_current_time_seconds();
    float total = engine->get_track_sequence_length_seconds();

    QString time_str = QString::asprintf("%.2f / %.2f s", current, total);
    track_sequence_length_label->setText(time_str);

    if (!engine->is_playing() && timer->isActive()) {
        timer->stop();

        if (!autoplay_flag) return;
        if (repeat_flag) {
            // No track selection logic here
        }
        else if (shuffle_flag) {
            std::size_t random_idx = std::rand() % track_selector->count();
            engine->set_track_sequence(random_idx);
            track_selector->setCurrentIndex(random_idx);
        }
        else {
            std::size_t next_idx = (track_selector->currentIndex() + 1) % track_selector->count();
            engine->set_track_sequence(next_idx);
            track_selector->setCurrentIndex(next_idx);
        }
        on_play_button_clicked();
    }
}

