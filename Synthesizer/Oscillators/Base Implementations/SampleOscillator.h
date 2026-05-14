#ifndef MIDI_PARSERSYNTHESIZER_SAMPLEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SAMPLEOSCILLATOR_H
#include <vector>

#include "Oscillator.h"

class SampleOscillator : public Oscillator {
private:
    const std::vector<float>* sample;
    float sample_index;
    float base_frequency;
    float playback_speed;

    void init(const std::vector<float>* sample = nullptr, float base_frequency = 440.0f);

public:
    SampleOscillator();
    SampleOscillator(const std::vector<float>* sample, float base_frequency);

    void set_frequency(float hz, float sample_rate) override;
    float get_sample() override;
};

#endif //MIDI_PARSERSYNTHESIZER_SAMPLEOSCILLATOR_H