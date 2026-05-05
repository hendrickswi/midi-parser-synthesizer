#include <filesystem>
#include <iostream>
#include <fstream>

#include <RtAudio.h>
#include <thread>

#include "FilePathSanitizer.h"
#include "Parser/MidiParser.h"
#include "Synthesizer/VoiceManager.h"

constexpr std::string auto_test_folder = "Testing files";
constexpr float sample_rate = 44100.0f;
constexpr unsigned int channels = 2;

int audio_callback(void *output_buffer, void *input_buffer, unsigned int num_frames, double stream_time, RtAudioStreamStatus status, void *user_data) {
    float *buffer = static_cast<float *>(output_buffer);
    VoiceManager *synth = static_cast<VoiceManager *>(user_data);
    synth->process_audio_buffer(buffer, num_frames);
    return 0;
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

    MidiParser parser = MidiParser();
    for (int i = 0; i < file_names.size(); i++) {
        parser.set_file(file_names[i]);
        std::cout << "Parsing file: " << file_names[i] << std::endl;
        auto sequence = TrackSequence();
        if (!parser.parse(sequence)) {
            std::cerr << "Error: Unable to parse the file" << std::endl;
            return 1;
        }
    }

    std::cout << std::endl << "Parsing completed! Press Enter to continue." << std::endl;
    std::string garbage_data;
    std::cin >> garbage_data;
    std::cout << std::endl;

    VoiceManager synth = VoiceManager(sample_rate, 75);
    RtAudio rt_audio;
    if (rt_audio.getDeviceCount() < 1) {
        std::cerr << "Error: No audio devices found" << std::endl;
        return 1;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = rt_audio.getDefaultOutputDevice();
    parameters.nChannels = channels;
    parameters.firstChannel = 0;

    unsigned int buffer_size = 1024;
    try {
        rt_audio.openStream(&parameters, nullptr,
            RTAUDIO_FLOAT32, sample_rate, &buffer_size, &audio_callback, &synth);
        rt_audio.startStream();
        std::cout << "Audio engine now running. Press Enter to quit." << std::endl;

        synth.note_on(0, 60, 100);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        synth.note_off(0, 60);
        std::cout << "Note released. Press Enter to exit" << std::endl;
        std::cin >> garbage_data;
        std::cout << std::endl;

        rt_audio.stopStream();
        rt_audio.closeStream();
    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}