#ifndef MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H
#include <memory>
#include "EnvelopeDecorator.h"

constexpr float TWO_PI = 2 * 3.14159265358979323846;

class TremoloEnvelope : public EnvelopeDecorator {
private:
    float current_phase;
    float phase_increment;

    float speed_hz;
    float depth;

public:
    TremoloEnvelope(std::unique_ptr<Envelope> env, float sample_rate = 44100.0, float speed_hz = 5.0, float depth = 0.5);
    ~TremoloEnvelope();

    float get_multiplier() override;
};


#endif //MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H