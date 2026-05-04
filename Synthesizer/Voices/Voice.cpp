#include "Voice.h"
#include "../Envelopes/Envelope.h"
#include "../Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
#include "../Oscillators/Base Implementations/Oscillator.h"

Voice::Voice() {
    oscillator = nullptr;
    envelope = nullptr;
    is_active = false;
    note_activation_time = std::chrono::high_resolution_clock::now();
}

Voice::Voice(std::unique_ptr<Oscillator> oscillator, std::unique_ptr<Envelope> envelope) {
    this->oscillator = std::move(oscillator);
    this->envelope = std::move(envelope);
    is_active = false;
    note_activation_time = std::chrono::high_resolution_clock::now();
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

void Voice::note_on(float hz, float sample_rate, float velocity) {
    if (!(oscillator && envelope)) return;
    oscillator->set_frequency(hz, sample_rate);
    envelope->on();
    current_volume = velocity;
    is_active = true;
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

    double raw_instruction = oscillator->get_sample();
    double multiplier = envelope->get_multiplier();
    return raw_instruction * multiplier * current_volume;
}