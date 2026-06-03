#ifndef MIDI_PARSERSYNTHESIZER_PATCHDEFINITION_H
#define MIDI_PARSERSYNTHESIZER_PATCHDEFINITION_H
#include <vector>
#include "../Synthesizer/Sample Loading/Sample.h"

constexpr unsigned int MAX_CHILDREN_IN_COMPOSITE = 4;

enum class OscillatorType {
    SAWTOOTH,
    SINE,
    SQUARE,
    TRIANGLE,
    COMPOSITE,
    NOISE,
    SAMPLE
};

enum class EnvelopeType {
    ADR,
    ADSR
};

enum class OscillatorDecoratorType {
    NONE,
    VIBRATO
};

enum class EnvelopeDecoratorType {
    NONE,
    TREMOLO
};

struct SawtoothOscillatorParams {
    float hz;
    float sample_rate;
};

struct SineOscillatorParams {
    float hz;
    float sample_rate;
};

struct SquareOscillatorParams {
    float hz;
    float sample_rate;
};

struct TriangleOscillatorParams {
    float hz;
    float sample_rate;
};

struct CompositeOscillatorParams {
    float hz;
    float sample_rate;
    OscillatorType child_types[MAX_CHILDREN_IN_COMPOSITE];
    float mix_volumes[MAX_CHILDREN_IN_COMPOSITE];
    float child_frequency_ratios[MAX_CHILDREN_IN_COMPOSITE];
    int num_children;
};

struct NoiseOscillatorParams {
};

struct SampleOscillatorParams {
    std::vector<Sample> samples;
    float target_sample_rate;
};

struct ADREnvelopeParams {
    float sample_rate;
    float attack_time;
    float attack_max;
    float decay_time;
    float release_time;
    float release_max;
    float release_min;
};

struct ADSREnvelopeParams {
    float sample_rate;
    float attack_time;
    float attack_max;
    float decay_time;
    float sustain_level;
    float release_time;
    float release_min;
};

struct VibratoDecoratorParams {
    float sample_rate;
    float base_hz;
    float speed_hz;
    float depth;
};

struct TremoloDecoratorParams {
    float sample_rate;
    float speed_hz;
    float depth;
};

struct PatchDefinition {
    // Oscillator params
    OscillatorType oscillator_type = OscillatorType::SINE;
    SawtoothOscillatorParams sawtooth_oscillator_params;
    SineOscillatorParams sine_oscillator_params;
    SquareOscillatorParams square_oscillator_params;
    TriangleOscillatorParams triangle_oscillator_params;
    CompositeOscillatorParams composite_oscillator_params;
    NoiseOscillatorParams noise_oscillator_params;
    SampleOscillatorParams sample_oscillator_params;

    // Envelope params
    EnvelopeType envelope_type = EnvelopeType::ADSR;
    ADREnvelopeParams adr_envelope_params;
    ADSREnvelopeParams adsr_envelope_params;

    // Oscillator decorator params
    OscillatorDecoratorType oscillator_decorator_type = OscillatorDecoratorType::NONE;
    VibratoDecoratorParams vibrato_decorator_params;

    // Envelope decorator params
    EnvelopeDecoratorType envelope_decorator_type = EnvelopeDecoratorType::NONE;
    TremoloDecoratorParams tremolo_decorator_params;

    bool is_one_shot = false;
    bool is_initialized = false;
};

#endif //MIDI_PARSERSYNTHESIZER_PATCHDEFINITION_H