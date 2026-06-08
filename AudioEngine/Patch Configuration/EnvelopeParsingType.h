#ifndef MIDI_PARSERSYNTHESIZER_ENVELOPETYPE_H
#define MIDI_PARSERSYNTHESIZER_ENVELOPETYPE_H

#include <nlohmann/json.hpp>

enum class ParsingEnvelopeType {
    UNKNOWN,
    ADSR,
    ADR,
    TREMOLO
};

NLOHMANN_JSON_SERIALIZE_ENUM(ParsingEnvelopeType, {
    {ParsingEnvelopeType::UNKNOWN, nullptr},
    {ParsingEnvelopeType::ADSR, "ADSR"},
    {ParsingEnvelopeType::ADR, "ADR"},
    {ParsingEnvelopeType::TREMOLO, "Tremolo"}
})



#endif //MIDI_PARSERSYNTHESIZER_ENVELOPETYPE_H