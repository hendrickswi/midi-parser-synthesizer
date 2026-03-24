#ifndef MIDI_PARSERSYNTHESIZER_MIDIEVENT_H
#define MIDI_PARSERSYNTHESIZER_MIDIEVENT_H
#include <cstdint>

/**
 * @struct MidiEvent
 * @brief Represents a single MIDI event (excluding Note On and Note Off) in a MIDI sequence.
 *
 * A structure defining the details of a single MIDI event, excluding musical notes, including its
 * timing within the track, the type of event, and relevant data payload.
 *
 * The event's specific details such as the meaning of @code data1@endcode and @code data2@endcode
 * depend on the type of event described by @code status@endcode.
 *
 * @note This structure is used to encapsulate MIDI events at a low level
 * and does not include higher-level sequencing or processing functionality.
 *
 * @note Do not use this structure for music notes. Use @code Note@endcode instead.
 *
 * @param absolute_time
 *        The time in ticks since the start of the track at which this event occurs.
 * @param status
 *        The status byte identifying the type of MIDI event
 * @param data1
 *        The first data byte, whose specific meaning depends on the event type.
 * @param data2
 *        The second data byte, whose specific meaning depends on the event type.
 */
struct MidiEvent {
    // Ticks since the start of the song
    uint32_t absolute_time;

    // Type of MIDI event (e.g., pitch bend, polyphonic key pressure)
    uint8_t status;

    // 8-bit data points about this event, varies based on the given status.
    uint8_t data1;
    uint8_t data2;
};

#endif //MIDI_PARSERSYNTHESIZER_MIDIEVENT_H