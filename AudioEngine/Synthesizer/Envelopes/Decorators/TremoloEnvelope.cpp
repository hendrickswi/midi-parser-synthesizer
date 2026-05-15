#include "TremoloEnvelope.h"

#include <cmath>

TremoloEnvelope::TremoloEnvelope(std::unique_ptr<Envelope> env, float sample_rate, float speed_hz, float depth)
: EnvelopeDecorator(std::move(env), sample_rate) {
        this->speed_hz = speed_hz;
        this->depth = depth;
        current_phase = 0;
        phase_increment = (speed_hz * TWO_PI) / sample_rate;
}

TremoloEnvelope::~TremoloEnvelope() = default;

float TremoloEnvelope::get_multiplier() {
        float base_multiplier = base_envelope->get_multiplier();
        float normalized_sine = std::sin(current_phase) * 0.5f + 0.5f;
        float tremolo_modifier = 1.0f - (normalized_sine * depth);

        // Wrap-around logic
        current_phase = fmod(current_phase + phase_increment, TWO_PI);
        return base_multiplier * tremolo_modifier;
}