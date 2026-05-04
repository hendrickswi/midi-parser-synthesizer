#ifndef MIDI_PARSERSYNTHESIZER_OSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_OSCILLATOR_H

constexpr double TWO_PI = 2 * 3.14159265358979323846;

inline double calculate_phase_increment(double hz, double sample_rate) {
    return TWO_PI * hz / sample_rate;
}

class Oscillator {
protected:
    double current_phase;
    double phase_increment;

    Oscillator();
    Oscillator(double hz, double sample_rate);
    Oscillator(const Oscillator& other);

public:
    virtual ~Oscillator();

    virtual double get_sample() = 0;
    virtual void set_frequency(double hz, double sample_rate);
};


#endif //MIDI_PARSERSYNTHESIZER_OSCILLATOR_H