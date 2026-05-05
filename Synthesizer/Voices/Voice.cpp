#include "Voice.h"

#include <cmath>

#include "../Envelopes/Envelope.h"
#include "../Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
#include "../Oscillators/Base Implementations/Oscillator.h"

static float pitch_to_hz(uint8_t pitch) {
    return 440.0f * std::pow(2.0f, ((float)pitch - 69) / 12.0f);
}

static float velocity_to_volume(uint8_t velocity) {
    return (float)velocity / 127.0f;
}

Voice::Voice() {
    oscillator = nullptr;
    envelope = nullptr;
    is_active = false;
    note_activation_time = std::chrono::high_resolution_clock::now();
    velocity = 0;
    channel = 0;
}

Voice::Voice(std::unique_ptr<Oscillator> oscillator, std::unique_ptr<Envelope> envelope) {
    this->oscillator = std::move(oscillator);
    this->envelope = std::move(envelope);
    is_active = false;
    note_activation_time = std::chrono::high_resolution_clock::now();
    velocity = 0;
    channel = 0;
}

Voice::~Voice() = default;

void Voice::set_oscillator(std::unique_ptr<Oscillator> oscillator) {
    this->oscillator = std::move(oscillator);
}
void Voice::set_envelope(std::unique_ptr<Envelope> envelope) {
    this->envelope = std::move(envelope);
}

[[nodiscard]] const std::chrono::high_resolution_clock::time_point& Voice::get_note_activation_time() const {
    return note_activation_time;
}

[[nodiscard]] const uint8_t& Voice::get_channel() const {
    return channel;
}

[[nodiscard]] const uint8_t& Voice::get_pitch() const {
    return pitch;
}

void Voice::note_on(uint8_t channel, uint8_t pitch, uint8_t velocity, float sample_rate) {
    if (!(oscillator && envelope)) return;

    this->channel = channel;
    this->pitch = pitch;
    this->velocity = velocity;
    volume = velocity_to_volume(velocity);

    oscillator->set_frequency(pitch_to_hz(pitch), sample_rate);
    envelope->on();

    is_active = true;
    note_activation_time = std::chrono::high_resolution_clock::now();
}

void Voice::note_off() {
    if (!envelope) return;
    envelope->off();
}

[[nodiscard]] bool Voice::is_free() const {
    if (!envelope) return true;
    return envelope->is_idle();
}

[[nodiscard]] bool Voice::is_released() const {
    if (!envelope) return true;
    return envelope->is_released();
}

float Voice::process() {
    if (!(is_active && oscillator && envelope)) return 0.0f;
    if (envelope->is_idle()) {
        is_active = false;
        return 0.0f;
    }

    float raw_instruction = oscillator->get_sample();
    float multiplier = envelope->get_multiplier();
    return raw_instruction * multiplier * volume;
}