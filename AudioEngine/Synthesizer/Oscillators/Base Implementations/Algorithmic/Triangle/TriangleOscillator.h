#ifndef MIDI_PARSERSYNTHESIZER_TRIANGLEOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_TRIANGLEOSCILLATOR_H

#include "../AlgorithmicOscillator.h"

class TriangleOscillator : public AlgorithmicOscillator {
public:
    TriangleOscillator();
    TriangleOscillator(float hz, float sample_rate);
    TriangleOscillator(const TriangleOscillator& other);

    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
};


#endif //MIDI_PARSERSYNTHESIZER_TRIANGLEOSCILLATOR_H