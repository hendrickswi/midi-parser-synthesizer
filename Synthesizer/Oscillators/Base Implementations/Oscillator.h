#ifndef MIDI_PARSERSYNTHESIZER_OSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_OSCILLATOR_H

constexpr float TWO_PI = 2 * 3.141592;

inline float calculate_phase_increment(float hz, float sample_rate) {
    return TWO_PI * hz / sample_rate;
}

class Oscillator {
protected:
    float current_phase;
    float phase_increment;

    Oscillator();
    Oscillator(float hz, float sample_rate);
    Oscillator(const Oscillator& other);

public:
    virtual ~Oscillator();

    virtual float get_sample() = 0;
    virtual void set_frequency(float hz, float sample_rate);
};


#endif //MIDI_PARSERSYNTHESIZER_OSCILLATOR_H