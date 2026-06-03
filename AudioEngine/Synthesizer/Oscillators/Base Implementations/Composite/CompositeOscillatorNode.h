#ifndef MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATORNODE_H
#define MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATORNODE_H
#include <variant>

#include "../Algorithmic/Sine/SineOscillator.h"
#include "../Algorithmic/Sawtooth/SawtoothOscillator.h"
#include "../Algorithmic/Square/SquareOscillator.h"
#include "../Algorithmic/Triangle/TriangleOscillator.h"
#include "../Noise/NoiseOscillator.h"

using OscillatorVariant = std::variant<SineOscillator, SawtoothOscillator, SquareOscillator, TriangleOscillator, NoiseOscillator>;

struct CompositeOscillatorNode {
    OscillatorVariant oscillator;
    float mix_volume;
    float frequency_ratio;
};

#endif //MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATORNODE_H