#ifndef MIDI_PARSERSYNTHESIZER_ENVELOPETYPE_H
#define MIDI_PARSERSYNTHESIZER_ENVELOPETYPE_H

#include <nlohmann/json.hpp>

enum class EnvelopeType {
    Unknown,
    ADSR,
    ADR,
    Tremolo
};

NLOHMANN_JSON_SERIALIZE_ENUM(EnvelopeType, {
    {EnvelopeType::Unknown, nullptr},
    {EnvelopeType::ADSR, "ADSR"},
    {EnvelopeType::ADR, "ADR"},
    {EnvelopeType::Tremolo, "Tremolo"}
})



#endif //MIDI_PARSERSYNTHESIZER_ENVELOPETYPE_H