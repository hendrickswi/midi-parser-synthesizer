#ifndef MIDI_PARSERSYNTHESIZER_SYSEXEVENT_H
#define MIDI_PARSERSYNTHESIZER_SYSEXEVENT_H

/**
 * @struct SysexEvent
 * @brief Represents a System Exclusive (Sysex) event in a MIDI file.
 *
 * A SysexEvent encapsulates data for a MIDI system-exclusive event,
 * including its timing, status, and any associated payload.
 *
 * @details
 * System Exclusive events are used in MIDI files to send device-specific
 * messages, whose format and content are determined by the specific device
 * or manufacturer.
 *
 * @param absolute_time
 *      Represents the number of ticks since the start of the track or song
 *      at which this event occurs.
 * @param status
 *      Specifies the status byte for the event, indicating the type of the
 *      Sysex message.
 * @param data
 *      Contains the payload of the Sysex event, which is specific to the type
 *      of message being sent.
 */
struct SysexEvent {
    uint32_t absolute_time;
    uint8_t status;
    std::vector<uint8_t> data;
};

#endif //MIDI_PARSERSYNTHESIZER_SYSEXEVENT_H