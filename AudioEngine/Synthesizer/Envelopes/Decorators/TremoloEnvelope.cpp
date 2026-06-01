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

void TremoloEnvelope::apply_to_block(float *buffer, unsigned int num_frames) {
    base_envelope->apply_to_block(buffer, num_frames);

    for (int i = 0; i < num_frames; i++) {
        float lfo_value = std::sin(current_phase);
        float tremolo_multiplier = 1.0f - (depth * 0.5f * (1.0f - lfo_value));
        buffer[i] *= tremolo_multiplier;

        if (current_phase >= TWO_PI) {
            current_phase -= TWO_PI;
        }
        current_phase += phase_increment;
    }
}
