#include "EnvelopeDecorator.h"

void EnvelopeDecorator::on() {
    base_envelope->on();
}

void EnvelopeDecorator::off() {
    base_envelope->off();
}

double EnvelopeDecorator::get_multiplier() {
    return base_envelope->get_multiplier();
}

bool EnvelopeDecorator::is_idle() const {
    return base_envelope->is_idle();
}

bool EnvelopeDecorator::is_released() const {
    return base_envelope->is_released();
}