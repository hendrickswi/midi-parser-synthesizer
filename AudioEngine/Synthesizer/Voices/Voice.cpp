#include "Voice.h"
#include "../Envelopes/Envelope.h"
#include "../Oscillators/Base Implementations/Base Oscillator/Oscillator.h"
#include "../../../EventTypeEnums/ContinuousControllers.h"
#include "../Sample Loading/Sample.h"
#include <cmath>
#include <iostream>

static float pitch_to_hz(uint8_t pitch) {
    return 440.0f * std::pow(2.0f, ((float)pitch - 69) / 12.0f);
}

static float byte_to_scale_float(uint8_t value) {
    float normalized = (float)value / 127.0f;
    return normalized * normalized;
}

void select_sample(const std::vector<Sample> &samples, uint8_t pitch, uint8_t velocity, const Sample*& selected_sample) {
    /*
     * Sample selection logic:
     *      1st pass: Try to find a sample with matching pitch *and* velocity
     *      2nd pass: Try to find a sample with matching pitch and any velocity
     *      Else: Use the first sample in the list of available samples (confirmed to not be empty)
     */

    if (samples.empty()) return;
    selected_sample = nullptr;

    for (const auto& sample : samples ) {
        if (pitch >= sample.min_pitch && pitch <= sample.max_pitch &&
            velocity >= sample.min_velocity && velocity <= sample.max_velocity) {
            selected_sample = &sample;
            break;
        }
    }

    if (selected_sample == nullptr) {
        for (const auto& sample : samples ) {
            if (pitch >= sample.min_pitch && pitch <= sample.max_pitch) {
                selected_sample = &sample;
                break;
            }
        }
    }

    if (selected_sample == nullptr) {
        selected_sample = &samples.front();
    }
}

Voice::Voice() { // NOLINT
    sawtooth_oscillator = SawtoothOscillator();
    sine_oscillator = SineOscillator();
    square_oscillator = SquareOscillator();
    triangle_oscillator = TriangleOscillator();
    composite_oscillator = CompositeOscillator();
    noise_oscillator = NoiseOscillator();
    sample_oscillator = SampleOscillator();
    adr_envelope = ADREnvelope();
    adsr_envelope = ADSREnvelope();
    vibrato_oscillator_decorator = VibratoOscillator();
    tremolo_envelope_decorator = TremoloEnvelope();

    active_oscillator = nullptr;
    active_envelope = nullptr;

    voice_buffer = std::vector<float>();
    is_active = false;
    note_activation_time = std::chrono::high_resolution_clock::now();
    pitch = 0;
    velocity = 0;
    channel = 0;
    volume = 1.0f;
    cc_volume = (100.0f / 127.0f) * (100.0f / 127.0f);
    cc_expression = 1.0f;
    key_held_flag = false;
    sustained_flag = false;
    one_shot_flag = false;
}

Voice::~Voice() = default;

[[nodiscard]] const std::chrono::high_resolution_clock::time_point& Voice::get_note_activation_time() const {
    return note_activation_time;
}

[[nodiscard]] const uint8_t& Voice::get_channel() const {
    return channel;
}

[[nodiscard]] const uint8_t& Voice::get_pitch() const {
    return pitch;
}

void Voice::configure_and_note_on(const PatchDefinition* config, uint8_t channel, uint8_t pitch, uint8_t velocity, float sample_rate) {
    if (!config) return;

    active_oscillator = nullptr;
    active_envelope = nullptr;

    this->channel = channel;
    this->pitch = pitch;
    this->velocity = velocity;
    volume = byte_to_scale_float(velocity);
    one_shot_flag = config->is_one_shot;

    float target_hz = pitch_to_hz(pitch);

    switch (config->oscillator_type) {
        case OscillatorType::SAMPLE : {
            const Sample* selected_sample = nullptr;
            select_sample(config->sample_oscillator_params.samples, pitch, velocity, selected_sample);

            sample_oscillator.set_sample(
                &selected_sample->audio_buffer,
                selected_sample->sample_rate,
                sample_rate,
                selected_sample->base_frequency,
                selected_sample->repeat_low,
                selected_sample->repeat_high
            );

            if (channel == 9) {
                target_hz = selected_sample->base_frequency;
            }

            active_oscillator = &sample_oscillator;
            break;
        }
        case OscillatorType::SINE : {
            active_oscillator = &sine_oscillator;
            break;
        }
        case OscillatorType::SQUARE : {
            active_oscillator = &square_oscillator;
            break;
        }
        case OscillatorType::SAWTOOTH : {
            active_oscillator = &sawtooth_oscillator;
            break;
        }
        case OscillatorType::TRIANGLE : {
            active_oscillator = &triangle_oscillator;
            break;
        }
        case OscillatorType::NOISE : {
            active_oscillator = &noise_oscillator;
            break;
        }
        case OscillatorType::COMPOSITE : {
            composite_oscillator.configure(config->composite_oscillator_params);
            active_oscillator = &composite_oscillator;
            break;
        }
        default : {
            std::cerr << "Warning: Unknown oscillator_type in passed in PatchDefinition* config in Voice::configure(...)." << std::endl;
            break;
        }
    }

    switch (config->envelope_type) {
        case EnvelopeType::ADR : {
            adr_envelope.set_params(
                sample_rate,
                config->adr_envelope_params.attack_time,
                config->adr_envelope_params.attack_max,
                config->adr_envelope_params.decay_time,
                config->adr_envelope_params.release_time,
                config->adr_envelope_params.release_max,
                config->adr_envelope_params.release_min
            );
            active_envelope = &adr_envelope;
            break;
        }
        case EnvelopeType::ADSR : {
            adsr_envelope.set_params(
                sample_rate,
                config->adsr_envelope_params.attack_time,
                config->adsr_envelope_params.attack_max,
                config->adsr_envelope_params.decay_time,
                config->adsr_envelope_params.sustain_level,
                config->adsr_envelope_params.release_time,
                config->adsr_envelope_params.release_min
            );
            active_envelope = &adsr_envelope;
            break;
        }
        default : {
            std::cerr << "Warning: Unknown envelope_type in passed in PatchDefinition* config in Voice::configure(...)." << std::endl;
            break;
        }
    }

    switch (config->oscillator_decorator_type) {
        case OscillatorDecoratorType::NONE : {
            break;
        }
        case OscillatorDecoratorType::VIBRATO : {
            vibrato_oscillator_decorator.set_params(
                sample_rate,
                config->vibrato_decorator_params.base_hz,
                config->vibrato_decorator_params.speed_hz,
                config->vibrato_decorator_params.depth
            );
            vibrato_oscillator_decorator.set_base_oscillator(active_oscillator);
            active_oscillator = &vibrato_oscillator_decorator;
            break;
        }
        default : {
            std::cerr << "Warning: Unknown oscillator_decorator_type in passed in PatchDefinition* config in Voice::configure(...)." << std::endl;
            break;
        }
    }

    switch (config->envelope_decorator_type) {
        case EnvelopeDecoratorType::NONE : {
            break;
        }
        case EnvelopeDecoratorType::TREMOLO : {
            tremolo_envelope_decorator.set_params(
                sample_rate,
                config->tremolo_decorator_params.speed_hz,
                config->tremolo_decorator_params.depth
            );
            tremolo_envelope_decorator.set_base_envelope(active_envelope),
            active_envelope = &tremolo_envelope_decorator;
            break;
        }
        default : {
            std::cerr << "Warning: Unknown envelope_decorator_type in passed in PatchDefinition* config in Voice::configure(...)." << std::endl;
            break;
        }
    }

    if (active_envelope && active_oscillator) {
        active_oscillator->set_frequency(target_hz, sample_rate);
        active_envelope->on();

        is_active = true;
        key_held_flag = true;
        note_activation_time = std::chrono::high_resolution_clock::now();
    }
}

void Voice::note_off() {
    if (!active_envelope) return;
    key_held_flag = false;

    if (!sustained_flag && !one_shot_flag) {
        active_envelope->off();
    }
}

void Voice::update_cc(uint8_t cc_number, uint8_t cc_value) {
    switch (cc_number) {
        case SUSTAIN_PEDAL : {
            sustained_flag = cc_value >= 64;
            if (!sustained_flag && !key_held_flag) {
                if (active_envelope) active_envelope->off();
            }
            break;
        }
        case MODULATION_WHEEL : {
            if (active_oscillator) {
                active_oscillator->set_modulation_depth(byte_to_scale_float(cc_value) * 10.0f);
            }
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
    if (active_envelope == nullptr) return true;
    return active_envelope->is_idle();
}

[[nodiscard]] bool Voice::is_released() const {
    if (active_envelope == nullptr) return false;
    return active_envelope->is_released();
}

[[nodiscard]] bool Voice::is_sustained() const {
    return sustained_flag;
}

[[nodiscard]] bool Voice::is_key_held() const {
    return key_held_flag;
}

void Voice::process_block(float* buffer, unsigned int num_frames) {
    if (!(is_active && active_oscillator && active_envelope)) return;
    if (active_envelope->is_idle()) {
        is_active = false;
        return;
    }

    voice_buffer.resize(num_frames);
    active_oscillator->process_sample_block(voice_buffer.data(), num_frames);
    active_envelope->apply_to_block(voice_buffer.data(), num_frames);
    for (int i = 0; i < num_frames; i++) {
        buffer[i] += voice_buffer[i] * volume * cc_volume * cc_expression;
    }
}