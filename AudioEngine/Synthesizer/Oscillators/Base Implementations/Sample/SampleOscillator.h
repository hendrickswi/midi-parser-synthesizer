#ifndef MIDI_PARSERSYNTHESIZER_SAMPLEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SAMPLEOSCILLATOR_H
#include <vector>

#include "../Base Oscillator/Oscillator.h"

class SampleOscillator : public Oscillator {
private:
    const std::vector<float>* sample;
    float raw_sample_rate;
    float target_sample_rate;
    float sample_index;
    float base_frequency;
    float playback_speed;
    int repeat_low_idx;
    int repeat_high_idx;
    bool repeat_enabled;

    void init(const std::vector<float>* sample, float raw_sample_rate, float target_sample_rate, float base_frequency, float* repeat_low, float* repeat_high);

public:
    SampleOscillator();
    SampleOscillator(const std::vector<float>* sample, float raw_sample_rate, float target_sample_rate, float base_frequency, float* repeat_low, float* repeat_high);

    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
    void set_modulation_depth(float depth) override;
    void set_frequency(float hz, float sample_rate) override;
};

#endif //MIDI_PARSERSYNTHESIZER_SAMPLEOSCILLATOR_H