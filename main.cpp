#include <filesystem>
#include <iostream>
#include <fstream>
#include <QApplication>
#include "AudioEngine.h"
#include "Qt/MainWindow.h"

constexpr std::string auto_test_folder = "Testing files";
constexpr float sample_rate = 44100.0f;
constexpr unsigned int num_channels = 1;
constexpr float volume = 0.5f;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    AudioEngine engine = AudioEngine(sample_rate, num_channels, volume);

    if (!std::filesystem::exists(auto_test_folder) || !std::filesystem::is_directory(auto_test_folder)) {
        std::cerr << "Error: The directory " << auto_test_folder << " cannot be resolved" << std::endl;
        return 1;
    }

    // Load all of the .mid files from the existent testing folder
    for (const auto& entry : std::filesystem::directory_iterator(auto_test_folder)) {
        if (entry.path().extension() == ".mid") {
            engine.load_midi_file(entry.path().string());
        }
    }

    MainWindow window = MainWindow(&engine);
    window.show();
    return app.exec();
}