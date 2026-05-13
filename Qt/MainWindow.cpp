#include "MainWindow.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include "../AudioEngine.h"
#include "../DirectoryManipulator.h"

MainWindow::MainWindow(AudioEngine* engine, QWidget *parent)
    : QMainWindow(parent), engine(engine) {
    setWindowTitle("MIDI Synthesizer");
    setMinimumSize(400, 200);

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    track_selector = new QComboBox(this);
    for (const auto& name : engine->get_loaded_file_names()) {
        track_selector->addItem(QString::fromStdString(name));
    }
    mainLayout->addWidget(track_selector);

    // creating time display
    track_sequence_length_label = new QLabel("0.00 / 0.00 s", this);
    track_sequence_length_label->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(track_sequence_length_label);

    // creating playback buttons
    QHBoxLayout* controls_layout = new QHBoxLayout();
    skip_back_button = new QPushButton("<< 5s", this);
    play_button = new QPushButton("Play", this);
    stop_button = new QPushButton("Stop", this);
    skip_fwd_button = new QPushButton("5s >>", this);

    // Add directory button
    add_directory_button = new QPushButton("Add Directory", this);

    controls_layout->addWidget(add_directory_button);
    controls_layout->addWidget(skip_back_button);
    controls_layout->addWidget(play_button);
    controls_layout->addWidget(stop_button);
    controls_layout->addWidget(skip_fwd_button);
    mainLayout->addLayout(controls_layout);
    setCentralWidget(centralWidget);

    // button to action mapping
    connect(play_button, &QPushButton::clicked, this, &MainWindow::on_play_button_clicked);
    connect(stop_button, &QPushButton::clicked, this, &MainWindow::on_stop_button_clicked);
    connect(skip_fwd_button, &QPushButton::clicked, this, &MainWindow::on_skip_fwd_button_clicked);
    connect(skip_back_button, &QPushButton::clicked, this, &MainWindow::on_skip_back_button_clicked);
    connect(track_selector, &QComboBox::currentIndexChanged, this, &MainWindow::on_track_selection_changed);
    connect(add_directory_button, &QPushButton::clicked, this, &MainWindow::on_add_directory_button_clicked);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update_timer);

    if (track_selector->count() > 0) {
        engine->set_track_sequence(0);
        update_timer();
    }
}

MainWindow::~MainWindow() = default;

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

void MainWindow::on_track_selection_changed(int index) {
    if (index < 0 || index >= engine->get_loaded_file_names().size()) return;
    engine->stop();
    engine->set_track_sequence(index);
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

void MainWindow::update_timer() {
    float current = engine->get_track_sequence_current_time_seconds();
    float total = engine->get_track_sequence_length_seconds();

    QString time_str = QString::asprintf("%.2f / %.2f s", current, total);
    track_sequence_length_label->setText(time_str);

    if (!engine->is_playing() && timer->isActive()) {
        timer->stop();
    }
}

