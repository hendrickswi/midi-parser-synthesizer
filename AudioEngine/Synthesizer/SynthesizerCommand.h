#ifndef MIDI_PARSERSYNTHESIZER_SYNTHESIZERCOMMAND_H
#define MIDI_PARSERSYNTHESIZER_SYNTHESIZERCOMMAND_H
#include <cstdint>

enum class SynthesizerCommandType {
    NOTE_ON,
    NOTE_OFF,
    SET_CHANNEL_PATCH,
    SET_CHANNEL_PITCH_BEND,
    SET_CHANNEL_PRESSURE,
    SET_CONTROL_CHANGE,
    STOP_ALL_VOICES
};

struct SynthesizerCommand {
    SynthesizerCommandType type;
    uint8_t channel;
    uint8_t data1;
    uint8_t data2;
    uint64_t absolute_sample;

    SynthesizerCommand()
        : type(static_cast<SynthesizerCommandType>(0)), channel(0), data1(0), data2(0), absolute_sample(0) {
    }

    SynthesizerCommand(SynthesizerCommandType type, uint8_t channel, uint8_t data1, uint8_t data2, uint64_t absolute_sample)
        : type(type), channel(channel), data1(data1), data2(data2), absolute_sample(absolute_sample) {
    }
};

#endif //MIDI_PARSERSYNTHESIZER_SYNTHESIZERCOMMAND_H