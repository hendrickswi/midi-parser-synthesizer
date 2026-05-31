#include <filesystem>
#include <fstream>
#include <QApplication>

#include "DirectoryManipulator.h"
#include "AudioEngine/AudioEngine.h"
#include "Qt/MainWindow.h"
#include "Qt/PlaybackController.h"

const std::string auto_test_folder = get_sanitized_file_path("Testing files");
constexpr float fallback_sample_rate = 48000.0f;
constexpr unsigned int num_channels = 1;
constexpr float volume = 1.0f;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    AudioEngine engine = AudioEngine(fallback_sample_rate, num_channels, volume);
    PlaybackController playback_controller = PlaybackController(&engine);
    MainWindow window = MainWindow(&playback_controller);

    // Initial test folder loading
    playback_controller.load_directory(auto_test_folder);

    window.show();
    return app.exec();
}