#include <filesystem>
#include <iostream>
#include <fstream>

#include <RtAudio.h>
#include <thread>

#include "AudioEngine.h"
#include "FilePathSanitizer.h"
#include "Parser/MidiParser.h"
#include "Sequencer/MidiSequencer.h"
#include "Synthesizer/VoiceManager.h"

constexpr std::string auto_test_folder = "Testing files";
constexpr float sample_rate = 44100.0f;
constexpr unsigned int num_channels = 1;
constexpr unsigned int print_interval_ms = 100;

int audio_callback(void *output_buffer, void *input_buffer, unsigned int num_frames, double stream_time, RtAudioStreamStatus status, void *user_data) {
    float *buffer = static_cast<float *>(output_buffer);
    VoiceManager *synth = static_cast<VoiceManager *>(user_data);
    synth->process_audio_buffer(buffer, num_frames);
    return 0;
}

void print_timer(float current_time, float total_time) {
    float elapsed_time = current_time;
    if (elapsed_time > total_time) {
        elapsed_time = total_time;
    }
    std::cout << "\r" << elapsed_time << " / " << total_time << " s" << std::flush;
}

int main() {
    auto file_names = std::vector<std::string>();
    std::cout << "MIDI parser and synthesizer program" << std::endl << std::endl;

    // Determine if user wants an automatic test or user input test
    std::cout << "Automatic test (1) or user input test (2)?";
    int test_type;
    std::cin >> test_type;
    std::cout << std::endl;

    if (test_type == 2) {
        // Get file name
        std::cout << "Enter the file name: ";
        std::string file_name;
        std::getline(std::cin >> std::ws, file_name);
        std::cout << std::endl;

        sanitize_file_path(file_name);
        file_names.push_back(file_name);
    }
    else {
        if (!std::filesystem::exists(auto_test_folder) || !std::filesystem::is_directory(auto_test_folder)) {
            std::cerr << "Error: The directory " << auto_test_folder << " cannot be resolved" << std::endl;
            return 1;
        }

        // Get all the .mid file names from the auto testing directory
        for (const auto& entry : std::filesystem::directory_iterator(auto_test_folder)) {
            if (entry.path().extension() == ".mid") {
                file_names.push_back(entry.path().string());
            }
        }
    }

    AudioEngine engine = AudioEngine(sample_rate, num_channels, 0.5f);
    std::cout << "Loading MIDI files..." << std::endl;
    for (int i = 0; i < file_names.size(); i++) {
        engine.load_midi_file(file_names[i]);
        std::cout << i + 1 << ". " << file_names[i] << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Which file would you like to listen to? (1-" + std::to_string(file_names.size()) + ")";
    int raw_file_index;
    std::cin >> raw_file_index;
    int file_index = raw_file_index - 1;
    if (file_index < 0 || file_index >= file_names.size()) {
        std::cerr << "Error: Invalid file index" << std::endl;
        return 1;
    }

    engine.set_track_sequence(file_index);
    engine.play();

    while (engine.is_playing()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(print_interval_ms));
        print_timer(engine.get_track_sequence_current_time_seconds(), engine.get_track_sequence_length_seconds());
    }

    engine.stop();
    return 0;
}