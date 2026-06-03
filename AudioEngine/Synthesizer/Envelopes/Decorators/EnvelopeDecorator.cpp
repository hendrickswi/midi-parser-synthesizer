#include "EnvelopeDecorator.h"

EnvelopeDecorator::EnvelopeDecorator() {
    base_envelope = nullptr;
    sample_rate = 48000.0f;
}

EnvelopeDecorator::~EnvelopeDecorator() = default;

void EnvelopeDecorator::on() {
    if (base_envelope == nullptr) return;
    base_envelope->on();
}

void EnvelopeDecorator::off() {
    if (base_envelope == nullptr) return;
    base_envelope->off();
}

void EnvelopeDecorator::apply_to_block(float* buffer, unsigned int num_frames) {
    if (base_envelope == nullptr) return;
    base_envelope->apply_to_block(buffer, num_frames);
}

bool EnvelopeDecorator::is_idle() const {
    if (base_envelope == nullptr) return false;
    return base_envelope->is_idle();
}

bool EnvelopeDecorator::is_released() const {
    if (base_envelope == nullptr) return false;
    return base_envelope->is_released();
}

void EnvelopeDecorator::set_base_envelope(Envelope* envelope) {
    this->base_envelope = envelope;
}