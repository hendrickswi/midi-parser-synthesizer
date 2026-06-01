#ifndef MIDI_PARSERSYNTHESIZER_OSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_OSCILLATOR_H

class Oscillator {
public:
    virtual ~Oscillator();

    virtual void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer = nullptr) = 0;
    virtual void set_modulation_depth(float depth) = 0;
    virtual void set_frequency(float hz, float sample_rate) = 0;
};


#endif //MIDI_PARSERSYNTHESIZER_OSCILLATOR_H