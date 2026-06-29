#ifndef MIDI_PARSERSYNTHESIZER_OSCILLATORPARSINGTYPE_H
#define MIDI_PARSERSYNTHESIZER_OSCILLATORPARSINGTYPE_H

#include <nlohmann/json.hpp>

enum class OscillatorParsingType {
    UNKNOWN,
    SAMPLE,
    SQUARE,
    SAWTOOTH,
    TRIANGLE,
    SINE,
    NOISE,
    COMPOSITE,
    VIBRATO,

    // Low pass filters
    CHAMBERLIN_SVF_LOWPASS,
    LTI_SVF_LOWPASS,

    // Band pass filters
    CHAMBERLIN_SVF_BANDPASS,
    LTI_SVF_BANDPASS,

    // High pass filters
    CHAMBERLIN_SVF_HIGHPASS,
    LTI_SVF_HIGHPASS
};

NLOHMANN_JSON_SERIALIZE_ENUM(OscillatorParsingType, {
    { OscillatorParsingType::UNKNOWN, nullptr },
    { OscillatorParsingType::SAMPLE, "SAMPLE" },
    { OscillatorParsingType::SQUARE, "SQUARE" },
    { OscillatorParsingType::SAWTOOTH, "SAWTOOTH" },
    { OscillatorParsingType::TRIANGLE, "TRIANGLE" },
    { OscillatorParsingType::SINE, "SINE" },
    { OscillatorParsingType::NOISE, "NOISE" },
    { OscillatorParsingType::COMPOSITE, "COMPOSITE" },
    { OscillatorParsingType::VIBRATO, "VIBRATO" },
    { OscillatorParsingType::CHAMBERLIN_SVF_LOWPASS, "CHAMBERLIN_SVF_LOWPASS" },
    { OscillatorParsingType::LTI_SVF_LOWPASS, "LTI_SVF_LOWPASS" },
    { OscillatorParsingType::CHAMBERLIN_SVF_BANDPASS, "CHAMBERLIN_SVF_BANDPASS" },
    { OscillatorParsingType::LTI_SVF_BANDPASS, "LTI_SVF_BANDPASS" },
    { OscillatorParsingType::CHAMBERLIN_SVF_HIGHPASS, "CHAMBERLIN_SVF_HIGHPASS" },
    { OscillatorParsingType::LTI_SVF_HIGHPASS, "LTI_SVF_HIGHPASS" }
});

#endif //MIDI_PARSERSYNTHESIZER_OSCILLATORPARSINGTYPE_H