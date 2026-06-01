#ifndef MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H

#include "../AlgorithmicOscillator.h"

class SineOscillator : public AlgorithmicOscillator {
public:
    SineOscillator();
    SineOscillator(float hz, float sample_rate);
    SineOscillator(const SineOscillator& other);

    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
};


#endif //MIDI_PARSERSYNTHESIZER_SINEOSCILLATOR_H