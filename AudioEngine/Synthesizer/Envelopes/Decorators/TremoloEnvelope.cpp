#include "TremoloEnvelope.h"

#include "../../Lookup Tables/SineLookupTable.h"

TremoloEnvelope::TremoloEnvelope() {
    // Default values
    speed_hz = 5.0f;
    depth = 0.5f;
    current_phase = 0;
    phase_increment = (speed_hz * TWO_PI) / sample_rate;
}

TremoloEnvelope::~TremoloEnvelope() = default;

void TremoloEnvelope::apply_to_block(float *buffer, unsigned int num_frames) {
    base_envelope->apply_to_block(buffer, num_frames);

    float table_size = static_cast<float>(SineLookupTable::TABLE_SIZE);
    for (int i = 0; i < num_frames; i++) {
        float normalized_phase = current_phase / TWO_PI;
        float lfo_value = SineLookupTable::sin(table_size * normalized_phase);
        float tremolo_multiplier = 1.0f - (depth * 0.5f * (1.0f - lfo_value));
        buffer[i] *= tremolo_multiplier;

        if (current_phase >= TWO_PI) {
            current_phase -= TWO_PI;
        }
        current_phase += phase_increment;
    }
}

void TremoloEnvelope::set_params(float sample_rate, float speed_hz, float depth) {
    this->sample_rate = sample_rate;
    this->speed_hz = speed_hz;
    this->depth = depth;
    phase_increment = (speed_hz * TWO_PI) / sample_rate;
}
