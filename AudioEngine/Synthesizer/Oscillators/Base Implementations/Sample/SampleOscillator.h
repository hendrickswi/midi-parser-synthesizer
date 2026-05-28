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
    float repeat_low;
    float repeat_high;

    void init(const std::vector<float>* sample, float raw_sample_rate, float target_sample_rate, float base_frequency, float* repeat_low, float* repeat_high);

public:
    SampleOscillator();
    SampleOscillator(const std::vector<float>* sample, float raw_sample_rate, float target_sample_rate, float base_frequency, float* repeat_low, float* repeat_high);

    float get_sample() override;
    void set_modulation_depth(float depth) override;
    void set_frequency(float hz, float sample_rate) override;
};

#endif //MIDI_PARSERSYNTHESIZER_SAMPLEOSCILLATOR_H