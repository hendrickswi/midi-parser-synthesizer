#ifndef MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATORNODE_H
#define MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATORNODE_H
#include <memory>

class Oscillator;

struct CompositeOscillatorNode {
    std::unique_ptr<Oscillator> oscillator;
    float mix_volume;
    float frequency_ratio;
};

#endif //MIDI_PARSERSYNTHESIZER_COMPOSITEOSCILLATORNODE_H