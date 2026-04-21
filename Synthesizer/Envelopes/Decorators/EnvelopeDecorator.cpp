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

const EnvelopeState& EnvelopeDecorator::get_state() const {
    return base_envelope->get_state();
}