#include "TremoloEnvelope.h"

#include <cmath>

TremoloEnvelope::TremoloEnvelope(std::unique_ptr<Envelope> env, double sample_rate, double speed_hz, double depth)
        : EnvelopeDecorator(std::move(env), sample_rate) {
        this->speed_hz = speed_hz;
        this->depth = depth;
        current_phase = 0;
        phase_increment = (speed_hz * TWO_PI) / sample_rate;
}

TremoloEnvelope::~TremoloEnvelope() = default;

double TremoloEnvelope::get_multiplier() {
        double base_multiplier = base_envelope->get_multiplier();
        double normalized_sine = std::sin(current_phase) * 0.5 + 0.5;
        double tremolo_modifier = 1.0 - (normalized_sine * depth);

        // Wrap-around logic
        current_phase = fmod(current_phase + phase_increment, TWO_PI);
        return base_multiplier * tremolo_modifier;
}