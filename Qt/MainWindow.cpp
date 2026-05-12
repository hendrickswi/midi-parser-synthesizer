#include "MainWindow.h"
#include <QHBoxLayout>
#include "../AudioEngine.h"

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
    QHBoxLayout* controlsLayout = new QHBoxLayout();
    skip_back_button = new QPushButton("<< 5s", this);
    play_button = new QPushButton("Play", this);
    stop_button = new QPushButton("Stop", this);
    skip_fwd_button = new QPushButton("5s >>", this);

    controlsLayout->addWidget(skip_back_button);
    controlsLayout->addWidget(play_button);
    controlsLayout->addWidget(stop_button);
    controlsLayout->addWidget(skip_fwd_button);
    mainLayout->addLayout(controlsLayout);
    setCentralWidget(centralWidget);

    // button to action mapping
    connect(play_button, &QPushButton::clicked, this, &MainWindow::on_play_button_clicked);
    connect(stop_button, &QPushButton::clicked, this, &MainWindow::on_stop_button_clicked);
    connect(skip_fwd_button, &QPushButton::clicked, this, &MainWindow::on_skip_fwd_button_clicked);
    connect(skip_back_button, &QPushButton::clicked, this, &MainWindow::on_skip_back_button_clicked);
    connect(track_selector, &QComboBox::currentIndexChanged, this, &MainWindow::on_track_selection_changed);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update_timer);

    if (track_selector->count() > 0) {
        engine->set_track_sequence(0);
        update_timer();
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::on_play_button_clicked() {
    engine->play();

    // Update every 33 ms (~30 per sec)
    timer->start(33);
}

void MainWindow::on_stop_button_clicked() {
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

void MainWindow::update_timer() {
    float current = engine->get_track_sequence_current_time_seconds();
    float total = engine->get_track_sequence_length_seconds();

    QString time_str = QString::asprintf("%.2f / %.2f s", current, total);
    track_sequence_length_label->setText(time_str);

    if (!engine->is_playing() && timer->isActive()) {
        timer->stop();
    }
}

