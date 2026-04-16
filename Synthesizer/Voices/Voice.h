#ifndef MIDI_PARSERSYNTHESIZER_VOICE_H
#define MIDI_PARSERSYNTHESIZER_VOICE_H
#include <memory>
#include <chrono>
#include "../Oscillators/Oscillator.h"


/**
 * Represents an envelope that defines the amplitude characteristics
 * of a sound over its duration.
 *
 * This is an abstract class that provides the interface for
 * controlling the behavior of the amplitude envelope.
 */
class Envelope;

class Voice {
private:
    // Need std::unique_ptr as these are abstract classes
    // (i.e., cannot instantiate an abstract class)
    std::unique_ptr<Oscillator> oscillator;
    std::unique_ptr<Envelope> envelope;

    bool is_active;
    std::chrono::high_resolution_clock::time_point note_activation_time;
    float current_volume = 0.0f;

public:
    Voice();
    Voice(std::unique_ptr<Oscillator> oscillator, std::unique_ptr<Envelope> envelope);
    Voice(const Voice& other) = delete;

    Voice& operator=(const Voice& other) = delete;

    /**
     * Sets the oscillator for the voice object.
     *
     * Replaces the current oscillator with the provided oscillator instance.
     * Takes ownership of the oscillator using a unique pointer.
     *
     * @param oscillator A unique pointer to an Oscillator object that will
     *                   be used for sound generation.
     */
    void set_oscillator(std::unique_ptr<Oscillator> oscillator);

    /**
     * Sets the envelope for the voice object.
     *
     * Replaces the current envelope with the provided envelope instance.
     * Takes ownership of the envelope using a unique pointer.
     *
     * @param envelope A unique pointer to an Envelope object that will
     *                 control the amplitude shape of the voice over time.
     */
    void set_envelope(std::unique_ptr<Envelope> envelope);

    [[nodiscard]] const std::chrono::high_resolution_clock::time_point& get_note_activation_time() const;

    void note_on(float hz, float sample_rate, float velocity);
    void note_off();
    [[nodiscard]] bool is_free() const;
    [[nodiscard]] bool is_released() const;
    float process();
};


#endif //MIDI_PARSERSYNTHESIZER_VOICE_H