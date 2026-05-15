extern "C" {
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
}

#include "SampleLoader.h"
#include <iostream>

SampleLoader::SampleLoader() = default;

std::vector<float> SampleLoader::load_wav_mono(const std::string& file_path) {
    std::vector<float> sample_data = std::vector<float>();
    unsigned int channels;
    unsigned int sample_rate;
    drwav_uint64 total_pcm_frame_count;

    float* data = drwav_open_file_and_read_pcm_frames_f32(file_path.c_str(), &channels,
        &sample_rate, &total_pcm_frame_count, nullptr);

    if (data == nullptr) {
        std::cerr << "Failed to load sample data at file path " << file_path << std::endl;
        return sample_data;
    }

    sample_data.reserve(total_pcm_frame_count);

    if (channels == 1) {
        for (unsigned int i = 0; i < total_pcm_frame_count; i++) {
            sample_data.push_back(data[i]);
        }
    }
    else if (channels == 2) {
        // Average left and right channels
        for (unsigned int i = 0; i < total_pcm_frame_count; i++) {
            sample_data.push_back((data[i * 2] + data[i * 2 + 1]) * 0.5f);
        }
    }

    drwav_free(data, nullptr); // Free memory allocated by dr_wav
    return sample_data;
}