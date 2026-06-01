#ifndef MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H
#include "../AlgorithmicOscillator.h"

class SquareOscillator : public AlgorithmicOscillator {

private:
    float poly_blep(float t, float dt);

public:
    SquareOscillator();
    SquareOscillator(float hz, float sample_rate);
    SquareOscillator(const SquareOscillator& other);

    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
};


#endif //MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H