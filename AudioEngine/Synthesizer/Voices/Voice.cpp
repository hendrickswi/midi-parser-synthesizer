#include "Voice.h"
#include "../Envelopes/Envelope.h"
#include "../Oscillators/Base Implementations/Base Oscillator/Oscillator.h"
#include "../../../EventTypeEnums/ContinuousControllers.h"
#include <cmath>

static float pitch_to_hz(uint8_t pitch) {
    return 440.0f * std::pow(2.0f, ((float)pitch - 69) / 12.0f);
}

static float byte_to_scale_float(uint8_t value) {
    float normalized = (float)value / 127.0f;
    return normalized * normalized;
}

void Voice::init(std::unique_ptr<Oscillator> oscillator, std::unique_ptr<Envelope> envelope) {
    this->oscillator = std::move(oscillator);
    this->envelope = std::move(envelope);
    is_active = false;
    note_activation_time = std::chrono::high_resolution_clock::now();
    velocity = 0;
    channel = 0;
    volume = 1.0f;
    cc_volume = (100.0f / 127.0f) * (1.0f / 127.0f);
    cc_expression = 1.0f;
    key_held_flag = false;
    sustained_flag = false;
    one_shot_flag = false;
}

Voice::Voice() { // NOLINT
    init();
}

Voice::Voice(std::unique_ptr<Oscillator> oscillator, std::unique_ptr<Envelope> envelope) { // NOLINT
    init(std::move(oscillator), std::move(envelope));
}

Voice::~Voice() = default;

void Voice::set_oscillator(std::unique_ptr<Oscillator> oscillator) {
    this->oscillator = std::move(oscillator);
}

void Voice::set_envelope(std::unique_ptr<Envelope> envelope) {
    this->envelope = std::move(envelope);
}

void Voice::set_one_shot(bool is_one_shot) {
    one_shot_flag = is_one_shot;
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
    volume = byte_to_scale_float(velocity);

    oscillator->set_frequency(pitch_to_hz(pitch), sample_rate);
    envelope->on();

    is_active = true;
    key_held_flag = true;
    note_activation_time = std::chrono::high_resolution_clock::now();
}

void Voice::note_off() {
    if (!envelope) return;
    key_held_flag = false;

    if (!sustained_flag && !one_shot_flag) {
        envelope->off();
    }
}

void Voice::update_cc(uint8_t cc_number, uint8_t cc_value) {
    switch (cc_number) {
        case SUSTAIN_PEDAL : {
            sustained_flag = cc_value >= 64;
            if (!sustained_flag && !key_held_flag) {
                if (envelope) envelope->off();
            }
            break;
        }
        case MODULATION_WHEEL : {
            oscillator->set_modulation_depth(byte_to_scale_float(cc_value) * 10.0f);
            break;
        }
        case CHANNEL_VOLUME : {
            cc_volume = byte_to_scale_float(cc_value);
            break;
        }
        case EXPRESSION : {
            cc_expression = byte_to_scale_float(cc_value);
            break;
        }
        default : {
            break;
        }
    }
}

[[nodiscard]] bool Voice::is_free() const {
    if (!envelope) return true;
    return envelope->is_idle();
}

[[nodiscard]] bool Voice::is_released() const {
    if (!envelope) return true;
    return envelope->is_released();
}

[[nodiscard]] bool Voice::is_sustained() const {
    return sustained_flag;
}

[[nodiscard]] bool Voice::is_key_held() const {
    return key_held_flag;
}


void Voice::process_block(float* buffer, unsigned int num_frames) {
    if (!(is_active && oscillator && envelope)) return;
    if (envelope->is_idle()) {
        is_active = false;
        return;
    }

    oscillator->process_sample_block(voice_buffer.data(), num_frames);
    envelope->apply_to_block(voice_buffer.data(), num_frames);
    for (int i = 0; i < num_frames; i++) {
        buffer[i] += voice_buffer[i] * volume * cc_volume * cc_expression;
    }
}