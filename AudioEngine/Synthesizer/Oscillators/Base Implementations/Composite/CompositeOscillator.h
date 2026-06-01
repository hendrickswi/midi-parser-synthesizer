#ifndef MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATOR_H
#include <memory>
#include <vector>
#include "../Base Oscillator/Oscillator.h"
#include "CompositeOscillatorNode.h"


class CompositeOscillator : public Oscillator {
private:
    std::vector<CompositeOscillatorNode> children;
    float current_hz;
    float sample_rate;

public:
    CompositeOscillator();
    CompositeOscillator(float hz, float sample_rate);

    void add_oscillator(std::unique_ptr<Oscillator> oscillator, float mix_volume, float frequency_ratio);
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
    void set_modulation_depth(float depth) override;
    void set_frequency(float hz, float sample_rate) override;
};

#endif //MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATOR_H