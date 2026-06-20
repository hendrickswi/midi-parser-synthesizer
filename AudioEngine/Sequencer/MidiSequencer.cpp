#include "MidiSequencer.h"

#include <algorithm>
#include <iostream>
#include <thread>

#include "../../EventTypeEnums/MetaEventType.h"
#include "../../EventTypeEnums/MidiEventType.h"

[[nodiscard]] inline uint32_t calculate_mpqn(const uint32_t tempo) {
    return 60000000 / tempo;
}

[[nodiscard]] inline uint32_t calculate_mpt(const uint32_t mpqn, const uint16_t division) {
    return mpqn / division;
}

[[nodiscard]] inline uint64_t micros_to_samples(uint64_t micros, float sample_rate) {
    return static_cast<uint64_t>(micros / 1000000.0 * sample_rate);
}

uint32_t parse_mpt_from_tempo_event(const MetaEvent& tempo_event, uint16_t division) {
    if (tempo_event.type != TEMPO_SETTING) return 0;

    uint32_t mpqn = 0;
    for (auto& byte : tempo_event.data) {
        mpqn = (mpqn << 8) | byte;
    }
    return calculate_mpt(mpqn, division);
}

MidiSequencer::MidiSequencer(float sample_rate) { // NOLINT
    init(sample_rate);
}


void MidiSequencer::init(float sample_rate) {
    track_sequence = nullptr;
    synthesizer = nullptr;
    is_playing_flag.store(false, std::memory_order_relaxed);
    is_skipping_flag = false;
    midi_file_ended_flag = false;
    this->sample_rate = sample_rate;
    micros_per_tick = 0;
    current_tick = 0;
    micros_since_last_tick = 0;
    audio_anchor_micros = 0;
    total_elapsed_micros = 0;
    track_indices = std::vector<TrackIndices>();
    active_notes = std::priority_queue<ActiveNote, std::vector<ActiveNote>, std::greater<>>();
    tempo_events = std::vector<MetaEvent>();
}

void MidiSequencer::setup_for_new_track_sequence() {
    is_playing_flag.store(false, std::memory_order_relaxed);
    is_skipping_flag = false;
    midi_file_ended_flag = false;

    if (!track_sequence) return;

    micros_per_tick = calculate_mpt(calculate_mpqn(120), track_sequence->get_division());
    current_tick = 0;
    micros_since_last_tick = 0;
    audio_anchor_micros = 0;
    total_elapsed_micros = 0;

    track_indices.clear();
    // This also does default instantiation of TrackIndices structs
    track_indices.resize(track_sequence->get_tracks().size());

    while (!active_notes.empty()) active_notes.pop();

    // Cache all tempo change events in order for later usage in multiple methods
    tempo_events = std::vector<MetaEvent>();
    for (const auto& track : track_sequence->get_tracks()) {
        for (const auto& meta_event : track.get_meta_events()) {
            if (meta_event.type == TEMPO_SETTING) {
                tempo_events.push_back(meta_event);
            }
        }
    }
    std::sort(tempo_events.begin(), tempo_events.end(), [](const MetaEvent& a, const MetaEvent& b) {
        return a.absolute_time < b.absolute_time;
    });
}

void MidiSequencer::process_events(const Track& track, TrackIndices& indices) {
    // Meta event processing
    const auto& meta_events = track.get_meta_events();
    while (indices.meta_idx < meta_events.size()
        && meta_events[indices.meta_idx].absolute_time <= current_tick) {

        const auto& meta_event = meta_events[indices.meta_idx];
        indices.meta_idx++;

        if (meta_event.type == TEMPO_SETTING) {
            uint32_t mpqn = 0;
            for (auto& byte : meta_event.data) {
                mpqn = (mpqn << 8) | byte;
            }
            micros_per_tick = calculate_mpt(mpqn, track_sequence->get_division());
        }
    }

    uint64_t current_absolute_sample = micros_to_samples(total_elapsed_micros, sample_rate);

    // Midi event processing
    const auto& midi_events = track.get_midi_events();
    while (indices.midi_event_idx < midi_events.size()
        && midi_events[indices.midi_event_idx].absolute_time <= current_tick) {

        const auto& midi_event = midi_events[indices.midi_event_idx];
        indices.midi_event_idx++;

        if (is_skipping_flag) {
            // Case: Audio thread is paused, meaning any events pushed to the
            // LockFreeQueue will not be processed before unpausing.
            // => Need to process these directly, but only need the *last* state

            switch (midi_event.command) {
                case PROGRAM_CHANGE: {
                    synthesizer->set_channel_patch(midi_event.channel, midi_event.data1);
                    break;
                }
                case CONTROL_CHANGE : {
                    synthesizer->set_channel_cc(midi_event.channel, midi_event.data1, midi_event.data2);
                    break;
                }
                case PITCH_BEND : {
                    synthesizer->set_channel_pitch_bend(midi_event.channel, (midi_event.data1 & 0x7F) | ((midi_event.data2 & 0x7F) << 7));
                    break;
                }
                case CHANNEL_PRESSURE : {
                    synthesizer->set_channel_pressure(midi_event.channel, midi_event.data1);
                    break;
                }
                default : {
                    break;
                }
            }
        }
        else {
            // Allow audio thread to process these events through the LockFreeQueue
            switch (midi_event.command) {
                case PROGRAM_CHANGE: {
                    SynthesizerCommand command {
                        SynthesizerCommandType::SET_CHANNEL_PATCH,
                        midi_event.channel,
                        midi_event.data1,
                        midi_event.data2,
                        current_absolute_sample
                    };
                    synthesizer->push_to_command_queue(command);
                    break;
                }
                case CONTROL_CHANGE : {
                    SynthesizerCommand command {
                        SynthesizerCommandType::SET_CONTROL_CHANGE,
                        midi_event.channel,
                        midi_event.data1,
                        midi_event.data2,
                        current_absolute_sample
                    };
                    synthesizer->push_to_command_queue(command);
                    break;
                }
                case PITCH_BEND : {
                    SynthesizerCommand command {
                        SynthesizerCommandType::SET_CHANNEL_PITCH_BEND,
                        midi_event.channel,
                        midi_event.data1,
                        midi_event.data2,
                        current_absolute_sample
                    };
                    synthesizer->push_to_command_queue(command);
                    break;
                }
                case CHANNEL_PRESSURE : {
                    SynthesizerCommand command {
                        SynthesizerCommandType::SET_CHANNEL_PRESSURE,
                        midi_event.channel,
                        midi_event.data1,
                        midi_event.data2,
                        current_absolute_sample
                    };
                    synthesizer->push_to_command_queue(command);
                    break;
                }
                default : {
                    break;
                }
            }
        }
    }

    // Note processing
    const auto& notes = track.get_notes();
    while (indices.note_idx < notes.size()
        && notes[indices.note_idx].absolute_time <= current_tick) {

        const auto& note = notes[indices.note_idx];
        indices.note_idx++;

        if (!is_skipping_flag) {
            SynthesizerCommand command {
                SynthesizerCommandType::NOTE_ON,
                note.channel,
                note.pitch,
                note.volume,
                current_absolute_sample
            };
            synthesizer->push_to_command_queue(command);
            active_notes.emplace(note);
        }
    }

    // Sysex event processing
    const auto& sysex_events = track.get_sysex_events();
    while (indices.sysex_idx < sysex_events.size()
        && sysex_events[indices.sysex_idx].absolute_time <= current_tick) {

        // No-op for now
        indices.sysex_idx++;
    }
}

void MidiSequencer::skip_microseconds(uint64_t micros_to_skip) {
    const auto& tracks = track_sequence->get_tracks();
    while (micros_to_skip >= micros_per_tick) {
        current_tick++;
        micros_to_skip -= micros_per_tick;
        total_elapsed_micros += micros_per_tick;

        // Cull notes that finish playing during skipping
        while (!active_notes.empty() && active_notes.top().end_time <= current_tick) {
            active_notes.pop();
        }

        for (int i = 0; i < tracks.size(); i++) {
            process_events(tracks[i], track_indices[i]);
        }
    }

    // uint64_t target_micros = total_elapsed_micros + micros_to_skip;
    // uint64_t simulated_micros = 0;
    // uint32_t simulated_ticks = 0;
    // uint32_t current_mpt = calculate_mpt(calculate_mpqn(120), track_sequence->get_division());
    //
    // for (const auto& tempo_event : tempo_events) {
    //     if (tempo_event.absolute_time > simulated_ticks) {
    //         uint32_t ticks_to_event = tempo_event.absolute_time - simulated_ticks;
    //         uint64_t micros_to_event = static_cast<uint64_t>(ticks_to_event) * current_mpt;
    //
    //         if (simulated_micros + micros_to_event >= target_micros) {
    //             break;
    //         }
    //
    //         simulated_micros += micros_to_event;
    //         simulated_ticks += ticks_to_event;
    //     }
    // }
    //
    // if (current_mpt > 0) {
    //     simulated_ticks += (target_micros - simulated_micros) / current_mpt;
    // }
    //
    // current_tick = simulated_ticks;
    // total_elapsed_micros = simulated_micros;
    // micros_per_tick = current_mpt;
    //
    // const auto& tracks = track_sequence->get_tracks();
    // for (int i = 0; i < tracks.size(); i++) {
    //     process_events(tracks[i], track_indices[i]);
    // }
    //
    // // Cull notes that finish playing during skipping
    // while (!active_notes.empty() && active_notes.top().end_time <= current_tick) {
    //     active_notes.pop();
    // }
}

[[nodiscard]] bool MidiSequencer::has_more_events() const {
    auto& tracks = track_sequence->get_tracks();
    for (int i = 0; i < tracks.size(); i++) {
        const auto& track = tracks[i];
        const auto& indices = track_indices[i];

        if (indices.note_idx < track.get_notes().size() ||
            indices.midi_event_idx < track.get_midi_events().size() ||
            indices.meta_idx < track.get_meta_events().size() ||
            indices.sysex_idx < track.get_sysex_events().size()) {
            return true;
        }
    }

    return false;
}

float MidiSequencer::calculate_current_track_sequence_gain() const {
    if (!track_sequence) return 1.0f;

    auto all_notes = Track();
    for (const auto& track : track_sequence->get_tracks()) {
        for (const auto& note : track.get_notes()) {
            all_notes.add_note(note);
        }
    }
    all_notes.sort_notes();

    uint32_t max_intensity = 0;
    uint32_t current_intensity = 0;
    uint32_t temp_current_tick = 0;
    std::size_t current_note_idx = 0;

    // Use temporary collections to prevent conflicts with instance variables
    // in case this method is called when they are not empty
    // O(n*log(n)) algorithm
    auto temp_active_notes = std::priority_queue<ActiveNote, std::vector<ActiveNote>, std::greater<>>();
    const auto& notes = all_notes.get_notes();
    while (current_note_idx < notes.size()) {
        const auto& note = notes[current_note_idx];
        while (current_note_idx < notes.size() && temp_current_tick >= notes[current_note_idx].absolute_time) {
            temp_active_notes.emplace(note);
            current_intensity += note.volume;
            current_note_idx++;
        }

        while (!temp_active_notes.empty() && temp_current_tick >= temp_active_notes.top().end_time) {
            current_intensity -= temp_active_notes.top().volume;
            temp_active_notes.pop();
        }

        if (current_intensity > max_intensity) {
            max_intensity = current_intensity;
        }

        if (current_note_idx < notes.size()) {
            temp_current_tick = all_notes.get_notes()[current_note_idx].absolute_time;
        }
    }

    // Calculate gain multiplier
    float gain_multiplier = 1.0f;
    if (max_intensity > 0.0f) {
        gain_multiplier = 4.0f / (static_cast<float>(max_intensity) / 127.0f);
    }
    else {
        gain_multiplier = 1.0f;
    }

    // Prevent extreme boosting causing noise floor to become prominent
    if (gain_multiplier > 3.0f) {
        gain_multiplier = 3.0f;
    }

    // std::cout << "Calculated gain multiplier: " << gain_multiplier << std::endl; // debug
    return gain_multiplier;
}

void MidiSequencer::start(uint64_t audio_current_micros) {
    if (!synthesizer || !track_sequence) return;
    is_playing_flag.store(true, std::memory_order_relaxed);

    // Reanchor
    audio_anchor_micros = audio_current_micros - total_elapsed_micros;
}

void MidiSequencer::stop() {
    if (!synthesizer || !track_sequence) return;
    is_playing_flag.store(false, std::memory_order_relaxed);
}

void MidiSequencer::skip_forward(float seconds) {
    if (!synthesizer || !track_sequence || seconds <= 0) return;

    float duration_remaining = get_total_duration_seconds() - get_current_time_seconds();
    if (duration_remaining <= 0) return;
    if (duration_remaining < seconds) seconds = duration_remaining;

    is_skipping_flag = true;
    skip_microseconds(static_cast<uint64_t>(seconds * seconds_to_micros));
    is_skipping_flag = false;
}

void MidiSequencer::skip_backward(float seconds) {
    if (!synthesizer || !track_sequence || seconds <= 0) return;

    float duration_elapsed = get_current_time_seconds();
    if (duration_elapsed <= 0) return;
    if (duration_elapsed < seconds) seconds = duration_elapsed;

    uint64_t skip_amount = static_cast<uint64_t>(seconds * seconds_to_micros);

    uint64_t target_micros = 0;
    if (total_elapsed_micros > skip_amount) {
        target_micros = total_elapsed_micros - skip_amount;
    }

    // Wipe everything and start from the beginning
    setup_for_new_track_sequence();
    is_skipping_flag = true;
    skip_microseconds(target_micros);
    is_skipping_flag = false;
}

void MidiSequencer::update(uint64_t audio_target_micros) {
    if (!is_playing_flag.load(std::memory_order_relaxed) || !synthesizer || !track_sequence || midi_file_ended_flag) return;

    if (audio_target_micros < audio_anchor_micros) return;
    uint64_t micros_to_advance = audio_target_micros - audio_anchor_micros;

    // Remove expired notes
    while (!active_notes.empty() && active_notes.top().end_time <= current_tick) {
        auto& note = active_notes.top();
        SynthesizerCommand command {
            SynthesizerCommandType::NOTE_OFF,
            note.channel,
            note.pitch,
            note.volume,
            micros_to_samples(total_elapsed_micros, sample_rate)
        };
        synthesizer->push_to_command_queue(command);
        active_notes.pop();
    }

    // Add new notes by triggering events registered to the current tick
    while (total_elapsed_micros + micros_per_tick <= micros_to_advance) {
        current_tick++;
        total_elapsed_micros += micros_per_tick;

        // Go through each "track" (each instrument/part)
        const auto& tracks = track_sequence->get_tracks();
        for (int i = 0; i < tracks.size(); i++) {
            process_events(tracks[i], track_indices[i]);
        }
    }

    if (!has_more_events() && active_notes.empty()) {
        midi_file_ended_flag = true;
        is_playing_flag.store(false, std::memory_order_relaxed);
    }
}

void MidiSequencer::reset() {
    init(sample_rate);
}

const TrackSequence* const MidiSequencer::get_track_sequence() const {
    return track_sequence;
}

void MidiSequencer::set_track_sequence(TrackSequence* sequence) {
    track_sequence = sequence;
    setup_for_new_track_sequence();
    if (synthesizer) {
        synthesizer->set_static_gain(calculate_current_track_sequence_gain());
    }
}

const VoiceManager* const MidiSequencer::get_synthesizer() const {
    return synthesizer;
}

void MidiSequencer::set_synthesizer(VoiceManager* synth) {
    synthesizer = synth;
}

float MidiSequencer::get_sample_rate() const {
    return sample_rate;
}

void MidiSequencer::set_sample_rate(float sample_rate) {
    this->sample_rate = sample_rate;
}

bool MidiSequencer::is_playing() const {
    return is_playing_flag.load(std::memory_order_relaxed);
}

bool MidiSequencer::midi_file_ended() const {
    return midi_file_ended_flag;
}

float MidiSequencer::get_current_time_seconds() const {
    if (!track_sequence || current_tick == 0) return 0.0f;

    return total_elapsed_micros * micros_to_seconds;
}

float MidiSequencer::get_total_duration_seconds() const {
    if (!track_sequence) return 0.0f;

    // Find the end time farthest away from the start
    uint32_t greatest_end_time = 0;
    for (const auto& track : track_sequence->get_tracks()) {
        const auto& notes = track.get_notes();
        if (notes.empty()) continue;

        for (const auto& note : notes) {
            uint32_t end_time = note.absolute_time + note.duration;
            if (end_time > greatest_end_time) {
                greatest_end_time = end_time;
            }
        }
    }

    // Calculate duration by accumulating based on the current tempo (mpt)
    float duration_seconds = 0.0f;
    uint32_t prev_tempo_event_time = 0;
    uint32_t prev_mpt = micros_per_tick;
    for (const auto& tempo_event : tempo_events) {
        if (tempo_event.absolute_time > prev_tempo_event_time) {
            duration_seconds += (tempo_event.absolute_time - prev_tempo_event_time) * prev_mpt * micros_to_seconds;
        }

        // Set up for next iteration
        prev_tempo_event_time = tempo_event.absolute_time;
        prev_mpt = parse_mpt_from_tempo_event(tempo_event, track_sequence->get_division());
    }

    // Final accumulation
    if (prev_tempo_event_time < greatest_end_time) {
        duration_seconds += (greatest_end_time - prev_tempo_event_time) * prev_mpt * micros_to_seconds;
    }

    return duration_seconds;
}

uint64_t MidiSequencer::get_total_elapsed_micros() const {
    return total_elapsed_micros;
}