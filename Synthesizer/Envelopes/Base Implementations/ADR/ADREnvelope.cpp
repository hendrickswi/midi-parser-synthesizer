#include "ADREnvelope.h"

#include <algorithm>

void ADREnvelope::calculate_rates() {
    attack_increment = attack_max_level / (std::max(attack_time, 0.00001f) * std::max(sample_rate, 0.00001f));
    decay_increment = (attack_max_level - release_max_level) / (std::max(attack_time, 0.00001f) * std::max(sample_rate, 0.00001f));
    release_increment = (release_max_level - release_min_level) / (std::max(release_time, 0.00001f) * std::max(sample_rate, 0.00001f));
}

void ADREnvelope::init(float sample_rate, float attack_time, float attack_max_level, float decay_time, // NOLINT
    float release_time, float release_max_level, float release_min_level) {
    state = IDLE;
    current_multiplier = 0.0;
    this->sample_rate = sample_rate;

    this->attack_time = attack_time;
    this->attack_max_level = attack_max_level;
    this->decay_time = decay_time;
    this->release_time = release_time;
    this->release_max_level = release_max_level;
    this->release_min_level = release_min_level;

    calculate_rates();
}

ADREnvelope::ADREnvelope() { // NOLINT
    init();
}

ADREnvelope::ADREnvelope(float sample_rate, float attack_time, float attack_max_level, float decay_time, // NOLINT
        float release_time, float release_max_level, float release_min_level) {
    init(sample_rate, attack_time, attack_max_level, decay_time, release_time, release_max_level, release_min_level);
}

ADREnvelope::ADREnvelope(const ADREnvelope& other) {
    state = other.state;
    sample_rate = other.sample_rate;
    current_multiplier = other.current_multiplier;

    attack_time = other.attack_time;
    attack_max_level = other.attack_max_level;
    decay_time = other.decay_time;
    release_time = other.release_time;
    release_max_level = other.release_max_level;
    release_min_level = other.release_min_level;

    attack_increment = other.attack_increment;
    decay_increment = other.decay_increment;
    release_increment = other.release_increment;
}

void ADREnvelope::on() {
    state = ATTACK;
}

void ADREnvelope::off() {
    if (state == IDLE) return;
    state = RELEASE;

    // In case off() is called before attack or decay finishes
    release_increment = current_multiplier / (std::max(release_time, 0.00001f) * std::max(sample_rate, 0.00001f));
}

float ADREnvelope::get_multiplier() {
    switch (state) {
        case IDLE : {
            current_multiplier = 0.0;
            break;
        }
        case ATTACK : {
            current_multiplier += attack_increment;
            if (current_multiplier >= attack_max_level) {
                current_multiplier = attack_max_level;
                state = DECAY;
            }
            break;
        }
        case DECAY : {
            current_multiplier -= decay_increment;
            if (current_multiplier <= release_min_level) {
                current_multiplier = release_min_level;
                state = IDLE;
            }
            break;
        }
        case RELEASE : {
            current_multiplier -= release_increment;
            if (current_multiplier <= release_min_level) {
                current_multiplier = release_min_level;
                state = IDLE;
            }
            break;
        }
    }

    return current_multiplier;
}

bool ADREnvelope::is_idle() const {
    return state == IDLE;
}

bool ADREnvelope::is_released() const {
    return state == RELEASE;
}
