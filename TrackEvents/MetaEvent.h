#ifndef MIDI_PARSERSYNTHESIZER_METAEVENT_H
#define MIDI_PARSERSYNTHESIZER_METAEVENT_H

#include <vector>
#include <cstdint>

/**
 * @struct MetaEvent
 * @brief Represents a MIDI Meta Event, typically used to store non-MIDI data
 *        associated with a MIDI sequence.
 *
 * @param absolute_time
 *      The absolute time at which the event occurs in MIDI ticks.
 * @param type
 *      The type of the meta event, indicating its purpose (e.g., tempo, track name).
 * @param data
 *      The payload of the meta event, containing the actual data associated with the event.
 *      This has varying length.
 *
 * Meta events provide additional information about a MIDI file or sequence,
 * such as track names, tempo, time signatures, or other metadata.
 */
struct MetaEvent {
    uint32_t absolute_time;
    uint8_t type;
    std::vector<uint8_t> data;
};

#endif //MIDI_PARSERSYNTHESIZER_METAEVENT_H