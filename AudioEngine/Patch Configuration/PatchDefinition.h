#ifndef MIDI_PARSERSYNTHESIZER_PATCHDEFINITION_H
#define MIDI_PARSERSYNTHESIZER_PATCHDEFINITION_H
#include <algorithm>
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
    VIBRATO,
    CHAMBERLIN_LOWPASS,
    LTI_LOWPASS,
    CHAMBERLIN_BANDPASS,
    LTI_BANDPASS,
    CHAMBERLIN_HIGHPASS,
    LTI_HIGHPASS
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

    CompositeOscillatorParams() : hz(440.0f), sample_rate(48000.0f), num_children(0) {
        std::fill_n(child_types, MAX_CHILDREN_IN_COMPOSITE, OscillatorType::SINE);
        std::fill_n(mix_volumes, MAX_CHILDREN_IN_COMPOSITE, 0.25f);
        std::fill_n(child_frequency_ratios, MAX_CHILDREN_IN_COMPOSITE, 1.0f);
    }

    CompositeOscillatorParams(float hz, float sample_rate, std::vector<OscillatorType> child_types, std::vector<float> mix_volumes, std::vector<float> child_frequency_ratios, int num_children)
    : hz(hz), sample_rate(sample_rate) {
        this->num_children = std::clamp(num_children, 0, static_cast<int>(MAX_CHILDREN_IN_COMPOSITE));
        for (unsigned int i = 0; i < num_children; i++) {
            if (i < child_types.size()) {
                this->child_types[i] = child_types[i];
            }

            if (i < mix_volumes.size()) {
                this->mix_volumes[i] = mix_volumes[i];
            }

            if (i < child_frequency_ratios.size()) {
                this->child_frequency_ratios[i] = child_frequency_ratios[i];
            }
        }
    }
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

struct SVFLowpassFilterParams {
    float sample_rate;
    float cutoff_hz;
    float resonance;
};

struct SVFBandpassFilterParams {
    float sample_rate;
    float cutoff_hz;
    float resonance;
};

struct SVFHighpassFilterParams {
    float sample_rate;
    float cutoff_hz;
    float resonance;
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
    SVFLowpassFilterParams low_pass_filter_params;
    SVFBandpassFilterParams band_pass_filter_params;
    SVFHighpassFilterParams high_pass_filter_params;

    // Envelope decorator params
    EnvelopeDecoratorType envelope_decorator_type = EnvelopeDecoratorType::NONE;
    TremoloDecoratorParams tremolo_decorator_params;

    bool is_one_shot = false;
    bool oscillator_initialized = false;
    bool envelope_initialized = false;

    void clear() {
        oscillator_type = OscillatorType::SINE;
        oscillator_initialized = false;
        envelope_type = EnvelopeType::ADSR;
        envelope_initialized = false;
        oscillator_decorator_type = OscillatorDecoratorType::NONE;
        envelope_decorator_type = EnvelopeDecoratorType::NONE;
        is_one_shot = false;

        sample_oscillator_params.samples.clear();
    }
};

#endif //MIDI_PARSERSYNTHESIZER_PATCHDEFINITION_H