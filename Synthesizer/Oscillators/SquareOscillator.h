#ifndef MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H
#define MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H

class SquareOscillator {
private:
    double current_phase;
    double phase_increment;

public:
    SquareOscillator();
    SquareOscillator(double hz, double sample_rate);
    SquareOscillator(const SquareOscillator& other);

    double get_sample();
    void set_frequency(double hz, double sample_rate);
};


#endif //MIDI_PARSERSYNTHESIZER_SQUAREOSCILLATOR_H