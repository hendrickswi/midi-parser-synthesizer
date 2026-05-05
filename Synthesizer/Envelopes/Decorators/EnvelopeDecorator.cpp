#include "EnvelopeDecorator.h"

EnvelopeDecorator::EnvelopeDecorator(std::unique_ptr<Envelope> env, float sample_rate) {
    this->base_envelope = std::move(env);
    this->sample_rate = sample_rate;
}

EnvelopeDecorator::~EnvelopeDecorator() = default;

void EnvelopeDecorator::on() {
    base_envelope->on();
}

void EnvelopeDecorator::off() {
    base_envelope->off();
}

float EnvelopeDecorator::get_multiplier() {
    return base_envelope->get_multiplier();
}

bool EnvelopeDecorator::is_idle() const {
    return base_envelope->is_idle();
}

bool EnvelopeDecorator::is_released() const {
    return base_envelope->is_released();
}