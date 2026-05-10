#include <filesystem>
#include <iostream>
#include <fstream>

#include <RtAudio.h>
#include <thread>

#include "FilePathSanitizer.h"
#include "Parser/MidiParser.h"
#include "Sequencer/MidiSequencer.h"
#include "Synthesizer/VoiceManager.h"

constexpr std::string auto_test_folder = "Testing files";
constexpr float sample_rate = 44100.0f;
constexpr unsigned int channels = 1;
constexpr unsigned int print_interval_ms = 100;

int audio_callback(void *output_buffer, void *input_buffer, unsigned int num_frames, double stream_time, RtAudioStreamStatus status, void *user_data) {
    float *buffer = static_cast<float *>(output_buffer);
    VoiceManager *synth = static_cast<VoiceManager *>(user_data);
    synth->process_audio_buffer(buffer, num_frames);
    return 0;
}

void print_timer(std::chrono::high_resolution_clock::time_point start_time, std::chrono::high_resolution_clock::time_point current_time, float end_time) {
    std::chrono::duration<float> elapsed = current_time - start_time;
    if (elapsed.count() > end_time) {
        elapsed = std::chrono::duration<float>(end_time);
    }
    std::cout << "\r" << elapsed.count() << " / " << end_time << " s" << std::flush;
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
    auto sequences = std::vector<TrackSequence>();
    for (auto& file : file_names) {
        parser.set_file(file);
        std::cout << "Parsing file: " << file << std::endl;
        auto sequence = TrackSequence();
        if (!parser.parse(sequence)) {
            std::cerr << "Error: Unable to parse the file" << std::endl;
            return 1;
        }
        sequences.push_back(sequence);
    }

    std::cout << std::endl << "Parsing completed! Press Enter to continue." << std::endl;
    std::cin.ignore();
    std::cin.get();
    std::cout << std::endl;

    VoiceManager synth = VoiceManager(sample_rate, 0.5f);
    RtAudio rt_audio;
    if (rt_audio.getDeviceCount() < 1) {
        std::cerr << "Error: No audio devices found" << std::endl;
        return 1;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = rt_audio.getDefaultOutputDevice();
    parameters.nChannels = channels;
    parameters.firstChannel = 0;

    MidiSequencer sequencer = MidiSequencer();
    sequencer.set_synthesizer(&synth);
    unsigned int buffer_size = 1024;
    try {
        rt_audio.openStream(&parameters, nullptr,
            RTAUDIO_FLOAT32, sample_rate, &buffer_size, &audio_callback, &synth);
        rt_audio.startStream();

        std::cout << "Audio engine now running." << std::endl;

        while (rt_audio.isStreamRunning()) {
            std::cout << "Which track would you like to listen to?" << std::endl << std::endl;
            for (int i = 0; i < sequences.size(); i++) {
                std::cout << i + 1 << ". " << file_names[i] << std::endl;
            }

            int track_idx;
            bool picked = false;
            while (!picked) {
                std::cout << "Choose a number from the list above: " << std::endl;
                std::cin >> track_idx;
                track_idx--;
                if (track_idx >= 0 && track_idx < sequences.size()) {
                    picked = true;
                    sequencer.set_track_sequence(&sequences[track_idx]);
                }
            }

            sequencer.start();
            std::cout << std::endl << "Now playing: " << file_names[track_idx] << std::endl << std::endl;

            // Timing stuff
            std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
            float end_time = sequencer.get_total_duration_seconds();
            auto last_print_time = start_time;

            while (sequencer.is_playing()) {
                sequencer.update();

                auto current_time = std::chrono::high_resolution_clock::now();
                auto ms_since_print = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_print_time).count();
                if (ms_since_print > print_interval_ms) {
                    print_timer(start_time, current_time, end_time);
                    last_print_time = current_time;
                }
            }
            std::cout << std::endl << std::endl;
            sequencer.stop();
            synth.stop();

            std::cout << "\"" << file_names[track_idx] << "\" has finished playing." << std::endl << std::endl;
            std::cout << "Press q to quit, or any other character key to continue." << std::endl;
            char c;
            std::cin >> c;
            if (c == 'q') {
                break;
            }
        }

        rt_audio.stopStream();
        rt_audio.closeStream();
    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}