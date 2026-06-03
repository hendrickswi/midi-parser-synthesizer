#ifndef MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H
#include <vector>

#include "OscillatorDecorator.h"

class VibratoOscillator : public OscillatorDecorator {
private:
    float current_phase;
    float phase_increment;
    float sample_rate;

    float base_hz;
    float speed_hz;
    float depth;

    float TWO_PI = 2 * 3.141592;

    std::vector<float> calculated_lfo_buffer;

public:
    VibratoOscillator();
    ~VibratoOscillator();

    void set_params(float sample_rate, float base_hz, float speed_hz, float depth);
    void process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) override;
    void set_frequency(float hz, float sample_rate) override;
    void set_modulation_depth(float depth) override;
};

#endif //MIDI_PARSERSYNTHESIZER_VIBRATOOSCILLATOR_H