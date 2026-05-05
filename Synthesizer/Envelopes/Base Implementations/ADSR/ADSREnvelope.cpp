#include "ADSREnvelope.h"

#include <algorithm>

void ADSREnvelope::recalculate_rates() {
    attack_increment = 1.0f / (std::max(attack_time, 0.00001f) * std::max(sample_rate, 0.00001f));
    decay_increment = (1.0f - sustain_level) / (std::max(decay_time, 0.00001f) * std::max(sample_rate, 0.00001f));
    release_increment = sustain_level / (std::max(release_time, 0.00001f) * std::max(sample_rate, 0.00001f));
}

void ADSREnvelope::init(float sample_rate, float attack_time, float decay_time, float sustain_level, float release_time) { // NOLINT
    state = IDLE;
    current_multiplier = 0.0;
    this->sample_rate = sample_rate;
    this->attack_time = attack_time;
    this->decay_time = decay_time;
    this->sustain_level = sustain_level;
    this->release_time = release_time;
    recalculate_rates();
}

ADSREnvelope::ADSREnvelope() { // NOLINT
    init();
}

ADSREnvelope::ADSREnvelope(float sample_rate, float attack_time, float decay_time, float sustain_level, float release_time) { // NOLINT
    init(sample_rate, attack_time, decay_time, sustain_level, release_time);
}

ADSREnvelope::ADSREnvelope(const ADSREnvelope& other) {
    state = other.state;
    sample_rate = other.sample_rate;
    current_multiplier = other.current_multiplier;
    attack_time = other.attack_time;
    decay_time = other.decay_time;
    sustain_level = other.sustain_level;
    release_time = other.release_time;
    attack_increment = other.attack_increment;
    decay_increment = other.decay_increment;
    sustain_increment = other.sustain_increment;
    release_increment = other.release_increment;
}

void ADSREnvelope::on() {
    state = ATTACK;
}

void ADSREnvelope::off() {
    if (state == IDLE) return;
    state = RELEASE;

    // Recalculate release speed based on the current volume
    // in case off() is called before attack or decay finishes
    release_increment = current_multiplier / (std::max(release_time, 0.00001f) * std::max(sample_rate, 0.00001f));
}

float ADSREnvelope::get_multiplier() {
    // Switch statement for state logic
    // Not via traditional OOP approach for performance
    switch (state) {
        case IDLE: {
            current_multiplier = 0.0;
            break;
        }
        case ATTACK: {
            current_multiplier += attack_increment;
            if (current_multiplier >= 1.0) {
                current_multiplier = 1.0;
                state = DECAY;
            }
            break;
        }
        case DECAY: {
            current_multiplier -= decay_increment;
            if (current_multiplier <= sustain_level) {
                current_multiplier = sustain_level;
                state = SUSTAIN;
            }
            break;
        }
        case SUSTAIN: {
            current_multiplier = sustain_level;
            break;
        }
        case RELEASE : {
            current_multiplier -= release_increment;
            if (current_multiplier <= 0.0) {
                current_multiplier = 0.0;
                state = IDLE;
            }
            break;
        }

        // For safety in case ADSREnvelopeState enum is changed
        default : {
            current_multiplier = 0.0;
            break;
        }
    }

    return current_multiplier;
}

bool ADSREnvelope::is_idle() const {
    return state == IDLE;
}

bool ADSREnvelope::is_released() const {
    return state == RELEASE;
}


