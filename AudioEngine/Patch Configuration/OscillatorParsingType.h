#ifndef MIDI_PARSERSYNTHESIZER_OSCILLATORPARSINGTYPE_H
#define MIDI_PARSERSYNTHESIZER_OSCILLATORPARSINGTYPE_H

enum OscillatorParsingType {
    UNKNOWN,
    SAMPLE,
    SQUARE,
    SAWTOOTH,
    TRIANGLE,
    SINE,
    NOISE,
    COMPOSITE
};

NLOHMANN_JSON_SERIALIZE_ENUM(OscillatorParsingType, {
    { OscillatorParsingType::UNKNOWN, nullptr },
    { OscillatorParsingType::SAMPLE, "SAMPLE" },
    { OscillatorParsingType::SQUARE, "SQUARE" },
    { OscillatorParsingType::SAWTOOTH, "SAWTOOTH" },
    { OscillatorParsingType::TRIANGLE, "TRIANGLE" },
    { OscillatorParsingType::SINE, "SINE" },
    { OscillatorParsingType::NOISE, "NOISE" },
    { OscillatorParsingType::COMPOSITE, "COMPOSITE" }
});

#endif //MIDI_PARSERSYNTHESIZER_OSCILLATORPARSINGTYPE_H