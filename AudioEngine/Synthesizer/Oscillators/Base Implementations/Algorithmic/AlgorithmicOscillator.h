#ifndef MIDI_PARSERSYNTHESIZER_ALGORITHMICOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_ALGORITHMICOSCILLATOR_H
#include "../Base Oscillator/Oscillator.h"

constexpr float TWO_PI = 2 * 3.141592;
constexpr float PI = 3.141592;

inline float calculate_phase_increment(float hz, float sample_rate) {
    return TWO_PI * hz / sample_rate;
}

class AlgorithmicOscillator : public Oscillator {

private:
    void init(float hz = 440.0f, float sample_rate = 44100.0f);

protected:
    float base_hz;
    float sample_rate;
    float phase_increment;
    float current_phase;

    AlgorithmicOscillator();
    AlgorithmicOscillator(float hz, float sample_rate);

public:
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override = 0;
    void set_modulation_depth(float depth) override;
    void set_frequency(float hz, float sample_rate) override;
};


#endif //MIDI_PARSERSYNTHESIZER_ALGORITHMICOSCILLATOR_H