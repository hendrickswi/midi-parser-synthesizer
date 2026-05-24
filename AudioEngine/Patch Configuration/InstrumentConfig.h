#ifndef MIDI_PARSERSYNTHESIZER_INSTRUMENTCONFIG_H
#define MIDI_PARSERSYNTHESIZER_INSTRUMENTCONFIG_H
#include <vector>
#include <nlohmann/json.hpp>
#include "../Synthesizer/Sample Loading/Sample.h"

struct InstrumentConfig {
    std::vector<Sample> samples;
    nlohmann::json envelope_data;
};

#endif //MIDI_PARSERSYNTHESIZER_INSTRUMENTCONFIG_H