#ifndef MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H

#include "../AlgorithmicOscillator.h"

class SawtoothOscillator : public AlgorithmicOscillator {
private:
    float poly_blep(float t, float dt);

public:
    SawtoothOscillator();
    SawtoothOscillator(float hz, float sample_rate);
    SawtoothOscillator(const SawtoothOscillator& other);

    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
};


#endif //MIDI_PARSERSYNTHESIZER_SAWTOOTHOSCILLATOR_H