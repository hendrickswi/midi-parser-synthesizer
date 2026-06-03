#ifndef MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATOR_H
#include <array>
#include "../Base Oscillator/Oscillator.h"
#include "CompositeOscillatorNode.h"
#include "../../../../Patch Configuration/PatchDefinition.h"


class CompositeOscillator : public Oscillator {
private:
    std::array<CompositeOscillatorNode, MAX_CHILDREN_IN_COMPOSITE> children;
    int num_children;
    std::array<float, 4096> child_buffer; // Safe estimate

public:
    CompositeOscillator();
    CompositeOscillator(float hz, float sample_rate);

    void configure(const CompositeOscillatorParams& params, float base_hz = 440.0f, float sample_rate = 48000.0f);
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
    void set_modulation_depth(float depth) override;
    void set_frequency(float hz, float sample_rate) override;
};

#endif //MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATOR_H