#ifndef MIDI_PARSERSYNTHESIZER_ENVELOPETYPE_H
#define MIDI_PARSERSYNTHESIZER_ENVELOPETYPE_H

#include <nlohmann/json.hpp>

enum class EnvelopeParsingType {
    UNKNOWN,
    ADSR,
    ADR,
    TREMOLO
};

NLOHMANN_JSON_SERIALIZE_ENUM(EnvelopeParsingType, {
    { EnvelopeParsingType::UNKNOWN, nullptr },
    { EnvelopeParsingType::ADSR, "ADSR" },
    { EnvelopeParsingType::ADR, "ADR" },
    { EnvelopeParsingType::TREMOLO, "TREMOLO" }
})



#endif //MIDI_PARSERSYNTHESIZER_ENVELOPETYPE_H