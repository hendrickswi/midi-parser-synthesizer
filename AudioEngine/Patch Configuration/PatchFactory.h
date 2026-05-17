#ifndef MIDI_PARSERSYNTHESIZER_PATCHFACTORY_H
#define MIDI_PARSERSYNTHESIZER_PATCHFACTORY_H
#include <memory>
#include <nlohmann/json_fwd.hpp>

class ADREnvelope;
class ADSREnvelope;
class SineOscillator;
class SquareOscillator;
class TriangleOscillator;
class SawtoothOscillator;
class NoiseOscillator;
class SampleOscillator;
class Envelope;

using json = nlohmann::json;

class PatchFactory {
public:
    static std::unique_ptr<Envelope> create_envelope(const json& config, float sample_rate);
    static std::unique_ptr<ADSREnvelope> create_adsr_envelope(float sample_rate, float attack_time, float attack_max_level, float decay_time,
        float sustain_level, float release_time, float release_min_level);
    static std::unique_ptr<ADREnvelope> create_adr_envelope(float sample_rate, float attack_time, float attack_max_level, float decay_time,
        float release_time, float release_max_level, float release_min_level);
    static std::unique_ptr<SampleOscillator> create_sample_oscillator(const std::vector<float>* sample, float base_frequency);
    static std::unique_ptr<SineOscillator> create_sine_oscillator(float hz, float sample_rate);
    static std::unique_ptr<SquareOscillator> create_square_oscillator(float hz, float sample_rate);
    static std::unique_ptr<TriangleOscillator> create_triangle_oscillator(float hz, float sample_rate);
    static std::unique_ptr<SawtoothOscillator> create_sawtooth_oscillator(float hz, float sample_rate);
    static std::unique_ptr<NoiseOscillator> create_noise_oscillator();
};


#endif //MIDI_PARSERSYNTHESIZER_PATCHFACTORY_H