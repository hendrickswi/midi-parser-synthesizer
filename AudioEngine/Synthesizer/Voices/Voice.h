#ifndef MIDI_PARSERSYNTHESIZER_VOICE_H
#define MIDI_PARSERSYNTHESIZER_VOICE_H
#include <chrono>
#include "../Oscillators/Base Implementations/Algorithmic/Sawtooth/SawtoothOscillator.h"
#include "../Oscillators/Base Implementations/Algorithmic/Sine/SineOscillator.h"
#include "../Oscillators/Base Implementations/Algorithmic/Square/SquareOscillator.h"
#include "../Oscillators/Base Implementations/Algorithmic/Triangle/TriangleOscillator.h"
#include "../Oscillators/Base Implementations/Composite/CompositeOscillator.h"
#include "../Oscillators/Base Implementations/Noise/NoiseOscillator.h"
#include "../Oscillators/Base Implementations/Sample/SampleOscillator.h"
#include "../Oscillators/Decorators/VibratoOscillator.h"
#include "../Envelopes/Base Implementations/ADR/ADREnvelope.h"
#include "../Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
#include "../Envelopes/Decorators/TremoloEnvelope.h"
#include "../../Patch Configuration/PatchDefinition.h"

class Voice {
private:
    // All of the possible oscillators
    SawtoothOscillator sawtooth_oscillator;
    SineOscillator sine_oscillator;
    SquareOscillator square_oscillator;
    TriangleOscillator triangle_oscillator;
    CompositeOscillator composite_oscillator;
    NoiseOscillator noise_oscillator;
    SampleOscillator sample_oscillator;

    // All of the possible envelopes
    ADREnvelope adr_envelope;
    ADSREnvelope adsr_envelope;

    // All of the possible oscillator decorators
    VibratoOscillator vibrato_oscillator_decorator;

    // All of the possible envelope decorators
    TremoloEnvelope tremolo_envelope_decorator;

    // The currently active oscillator & envelope
    Oscillator* active_oscillator;
    Envelope* active_envelope;

    std::vector<float> voice_buffer;

    bool is_active;
    std::chrono::high_resolution_clock::time_point note_activation_time;
    uint8_t channel;
    uint8_t pitch;
    uint8_t velocity;
    float volume;
    float cc_volume;
    float cc_expression;
    bool key_held_flag;
    bool sustained_flag;
    bool one_shot_flag;

public:
    Voice();
    Voice(const Voice& other) = delete;

    ~Voice();

    Voice& operator=(const Voice& other) = delete;

    [[nodiscard]] const std::chrono::high_resolution_clock::time_point& get_note_activation_time() const;
    [[nodiscard]] const uint8_t& get_channel() const;
    [[nodiscard]] const uint8_t& get_pitch() const;

    void configure_and_note_on(const PatchDefinition* config, uint8_t channel, uint8_t pitch, uint8_t velocity, float sample_rate);
    void note_off();
    void update_cc(uint8_t cc_number, uint8_t cc_value);
    [[nodiscard]] bool is_free() const;
    [[nodiscard]] bool is_released() const;
    [[nodiscard]] bool is_sustained() const;
    [[nodiscard]] bool is_key_held() const;

    void process_block(float* buffer, unsigned int num_samples);
};


#endif //MIDI_PARSERSYNTHESIZER_VOICE_H