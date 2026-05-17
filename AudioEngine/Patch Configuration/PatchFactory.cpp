//
// Created by williamh on 5/17/26.
//

#include "PatchFactory.h"

#include <iostream>

#include "../Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
#include "../Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelope.h"
#include "../Synthesizer/Envelopes/Envelope.h"
#include "../Synthesizer/Envelopes/EnvelopeType.h"
#include "../Synthesizer/Envelopes/Decorators/TremoloEnvelope.h"
#include "../Synthesizer/Oscillators/Base Implementations/SampleOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/SineOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/SquareOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/TriangleOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/SawtoothOscillator.h"
#include "../Synthesizer/Oscillators/Base Implementations/NoiseOscillator.h"
#include <nlohmann/json.hpp>


std::unique_ptr<Envelope> PatchFactory::create_envelope(const json& config, float sample_rate) {
    if (config.is_null() || !config.contains("type")) {
        // Fallback
        return std::make_unique<ADSREnvelope>(sample_rate, 0.005f, 1.0f, 3.0f, 0.025f, 0.2f, 0.0f);
    }

    EnvelopeType type = config["type"];
    switch (type) {
        case EnvelopeType::ADSR : {
            return std::make_unique<ADSREnvelope>(
                sample_rate,
                config.value("attack_time", 0.005f),
                config.value("attack_max", 1.0f),
                config.value("decay_time", 3.0f),
                config.value("sustain_level", 0.025f),
                config.value("release_time", 0.2f),
                config.value("release_min", 0.0f)
            );
        }
        case EnvelopeType::ADR : {
            return std::make_unique<ADREnvelope>(
                sample_rate,
                config.value("attack_time", 0.005f),
                config.value("attack_max", 1.0f),
                config.value("decay_time", 0.1f),
                config.value("release_time", 0.1f),
                config.value("release_max", 0.1f),
                config.value("release_min", 0.0f)
            );
        }
        case EnvelopeType::Tremolo : {
            auto base_env = create_envelope(config["base_envelope"], sample_rate);
            return std::make_unique<TremoloEnvelope>(
                std::move(base_env),
                sample_rate,
                config.value("speed_hz", 5.0f),
                config.value("depth", 0.5f)
            );
        }
        case EnvelopeType::Unknown :
        default: {
            std::cerr << "Warning: Unknown EnvelopeType in selected json. ADSR envelope fallback will be used." << std::endl;
            return std::make_unique<ADSREnvelope>(
                sample_rate,
                0.005f,
                1.0f,
                3.0f,
                0.025f,
                0.2f,
                0.0f
            );
        }
    }
}

std::unique_ptr<ADSREnvelope> PatchFactory::create_adsr_envelope(float sample_rate, float attack_time,
    float attack_max_level, float decay_time, float sustain_level, float release_time, float release_min_level) {
    return std::make_unique<ADSREnvelope>(sample_rate, attack_time, attack_max_level, decay_time,
        sustain_level, release_time, release_min_level);
}

std::unique_ptr<ADREnvelope> PatchFactory::create_adr_envelope(float sample_rate, float attack_time,
    float attack_max_level, float decay_time, float release_time, float release_max_level, float release_min_level) {
    return std::make_unique<ADREnvelope>(sample_rate, attack_time, attack_max_level, decay_time,
        release_max_level, release_time, release_min_level);
}

std::unique_ptr<SampleOscillator> PatchFactory::create_sample_oscillator(const std::vector<float>* sample, float base_frequency) {
    return std::make_unique<SampleOscillator>(sample, base_frequency);
}

std::unique_ptr<SineOscillator> PatchFactory::create_sine_oscillator(float hz, float sample_rate) {
    return std::make_unique<SineOscillator>(hz, sample_rate);
}

std::unique_ptr<SquareOscillator> PatchFactory::create_square_oscillator(float hz, float sample_rate) {
    return std::make_unique<SquareOscillator>(hz, sample_rate);
}

std::unique_ptr<TriangleOscillator> PatchFactory::create_triangle_oscillator(float hz, float sample_rate) {
    return std::make_unique<TriangleOscillator>(hz, sample_rate);
}

std::unique_ptr<SawtoothOscillator> PatchFactory::create_sawtooth_oscillator(float hz, float sample_rate) {
    return std::make_unique<SawtoothOscillator>(hz, sample_rate);
}

std::unique_ptr<NoiseOscillator> PatchFactory::create_noise_oscillator() {
    return std::make_unique<NoiseOscillator>();
}