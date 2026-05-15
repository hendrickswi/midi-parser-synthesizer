#include <filesystem>
#include <iostream>
#include <fstream>
#include <QApplication>
#include "AudioEngine/AudioEngine.h"
#include "DirectoryManipulator.h"
#include "Qt/MainWindow.h"

constexpr std::string auto_test_folder = "Testing files";
constexpr float sample_rate = 44100.0f;
constexpr unsigned int num_channels = 1;
constexpr float volume = 0.5f;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    AudioEngine engine = AudioEngine(sample_rate, num_channels, volume);

    DirectoryManipulator dir_manipulator(auto_test_folder);
    auto midi_files = dir_manipulator.get_midi_files_in_directory();
    for (const auto& file : midi_files) {
        engine.load_midi_file(file.get_file_path());
    }

    MainWindow window = MainWindow(&engine);
    window.show();
    return app.exec();
}