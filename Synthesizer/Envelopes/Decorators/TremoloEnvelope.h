#ifndef MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H
#define MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H
#include <memory>
#include "EnvelopeDecorator.h"

constexpr double TWO_PI = 2 * 3.14159265358979323846;

class TremoloEnvelope : public EnvelopeDecorator {
private:
    double current_phase;
    double phase_increment;

    double speed_hz;
    double depth;

public:
    TremoloEnvelope(std::unique_ptr<Envelope> env, double sample_rate = 44100.0,
        double speed_hz = 5.0, double depth = 0.5);
    ~TremoloEnvelope();

    double get_multiplier() override;
};


#endif //MIDI_PARSERSYNTHESIZER_TREMOLOENVELOPE_H