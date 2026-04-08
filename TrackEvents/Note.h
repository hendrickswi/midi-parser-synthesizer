//
// Created by williamh on 3/23/26.
//

#ifndef MIDI_PARSERSYNTHESIZER_NOTE_H
#define MIDI_PARSERSYNTHESIZER_NOTE_H
#include <cstdint>

/**
 * @struct Note
 * @brief Represents a musical note in a song.
 *
 * The @code Note@endcode struct is used to encapsulate information about a single musical
 * note. This includes its absolute timing within the song, the duration
 * it lasts, the pitch, and its volume.
 *
 * @note Use this structure exclusively for representing musical notes in a song.
 *
 * @param absolute_time
 *      The number of ticks since the beginning of the song when the note starts.
 * @param duration
 *      The number of ticks the note lasts.
 * @param pitch
 *      The pitch value of the note, typically represented as a MIDI note number.\
 * @param volume
 *      The volume or intensity of the note.
 */
struct Note {
    // Ticks since the start of the song
    std::uint32_t absolute_time;

    // Ticks since the start of the note
    std::uint32_t duration;

    std::uint8_t pitch;
    std::uint8_t volume;

    // The channel (0-15)
    std::uint8_t channel;


};

#endif //MIDI_PARSERSYNTHESIZER_NOTE_H