#ifndef MIDI_PARSERSYNTHESIZER_VOICE_H
#define MIDI_PARSERSYNTHESIZER_VOICE_H
#include <memory>

#include "../Oscillators/Oscillator.h"

class Envelope;

class Voice {
private:
    // Need std::unique_ptr as these are abstract classes
    // (i.e., cannot instantiate an abstract class)
    std::unique_ptr<Oscillator> oscillator;
    std::unique_ptr<Envelope> envelope;

    bool is_active;
    float current_volume = 0.0f;

public:
    Voice();
    Voice(const Oscillator& oscillator, std::unique_ptr<Envelope> envelope);
    Voice(const Voice& other);

    void set_oscillator(const Oscillator& oscillator);
    void set_envelope(std::unique_ptr<Envelope> envelope);

    void note_on(float hz, float velocity);
    void note_off();
    bool is_free();
    void update();
};


#endif //MIDI_PARSERSYNTHESIZER_VOICE_H