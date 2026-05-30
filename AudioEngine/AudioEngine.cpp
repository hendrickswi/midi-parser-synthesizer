#include "AudioEngine.h"
#include "../EventTypeEnums/MidiEventType.h"

#include <iostream>
#include <set>

void AudioEngine::init(float sample_rate, unsigned int num_channels) {
    sequencer.set_synthesizer(&synth);

    // RtAudio setup
    if (rt_audio.getDeviceCount() < 1) {
        std::cerr << "Warning: No audio devices found" << std::endl;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = rt_audio.getDefaultOutputDevice();
    parameters.nChannels = num_channels;
    parameters.firstChannel = 0;
    unsigned int buffer_size = 1024;
    try {
        rt_audio.openStream(&parameters, nullptr,
            RTAUDIO_FLOAT32, sample_rate, &buffer_size, &audio_callback, &synth);
        rt_audio.startStream();

        std::cout << "Audio engine now running." << std::endl << std::endl;
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("AudioEngine::init() failed during RtAudio stream opening/starting. Error:") += e.what());
    }

    // Other instance variables
    loaded_track_sequences = std::vector<TrackSequence>();
    loaded_file_names = std::vector<std::string>();
    current_track = -1;
    file_has_switched = false;
    // Do not spawn a sequencer thread here; it will join instantly because the sequencer is not playing yet
}

int AudioEngine::audio_callback(void *output_buffer, void *input_buffer, unsigned int num_frames, double stream_time,
        RtAudioStreamStatus status, void *user_data) {
    float *buffer = static_cast<float *>(output_buffer);
    VoiceManager *synth = static_cast<VoiceManager *>(user_data);
    synth->process_audio_buffer(buffer, num_frames);
    return 0;
}

void AudioEngine::sequencer_thread_loop() {
    while (sequencer.is_playing()) {
        sequencer.update();

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

AudioEngine::AudioEngine() : parser(), sequencer(), synth() {
    init();
}

AudioEngine::AudioEngine(float sample_rate, unsigned int num_channels, float global_volume)
: parser(), sequencer(), synth(sample_rate, global_volume) {
    init(sample_rate, num_channels);
}

AudioEngine::~AudioEngine() {
    stop();
    if (!rt_audio.isStreamOpen()) return;
    if (rt_audio.isStreamRunning()) rt_audio.stopStream();
    rt_audio.closeStream();
}

bool AudioEngine::load_midi_file(const std::string& file_path) {
    if (std::find(loaded_file_names.begin(), loaded_file_names.end(), file_path) != loaded_file_names.end()) return false;
    parser.set_file(file_path);
    TrackSequence sequence;

    if (!parser.parse(sequence)) return false;
    loaded_track_sequences.push_back(sequence);
    loaded_file_names.push_back(file_path);
    return true;
}

const std::vector<std::string>& AudioEngine::get_loaded_file_names() const {
    return loaded_file_names;
}

std::size_t AudioEngine::get_current_track_sequence_index() const {
    return current_track;
}

std::vector<std::string> AudioEngine::get_instrument_names_of_current_track_sequence() const {
    const auto& track_sequence = loaded_track_sequences[current_track];
    auto melodic_patch_ids = std::set<uint8_t>();
    auto drum_patch_ids = std::set<uint8_t>();

    bool has_notes = false;
    bool explicit_program_change_found = false;

    // Find unique patch IDs
    for (auto& track : track_sequence.get_tracks()) {
        for (const auto& note : track.get_notes()) {
            if (note.channel == 9) {
                drum_patch_ids.insert(note.pitch);
            }
            else {
                has_notes = true;
            }
        }

        for (const auto& event : track.get_midi_events()) {
            if (event.command == PROGRAM_CHANGE && event.channel != 9) {
                melodic_patch_ids.insert(event.data1);
                explicit_program_change_found = true;
            }
        }
    }

    if (has_notes && !explicit_program_change_found) {
        // Grand piano default
        melodic_patch_ids.insert(0);
    }

    // Convert melodic patch IDs into instrument names
    std::vector<std::string> instrument_names = std::vector<std::string>();
    for (auto patch_id : melodic_patch_ids) {
        if (patch_id <= 127) {
            instrument_names.push_back(GM_MELODIC_PATCH_NAMES[patch_id]);
        }
    }

    // Convert drum patch pitches into instrument names
    for (auto pitch : drum_patch_ids) {
        if (pitch <= 127 && GM_DRUM_PATCH_NAMES[pitch] != "Unmapped") {
            instrument_names.push_back(GM_DRUM_PATCH_NAMES[pitch]);
        }
    }

    return instrument_names;
}

void AudioEngine::play() {
    if (current_track < 0 || current_track >= loaded_track_sequences.size() ||
        sequencer.is_playing()) return;

    // Thread cleanup
    if (sequencer_thread.joinable()) {
        sequencer_thread.join();
    }

    // Track setup
    if (file_has_switched) {
        sequencer.set_track_sequence(&loaded_track_sequences[current_track]);
        file_has_switched = false;
    }
    else if (sequencer.midi_file_ended()) {
        sequencer.reset();
    }

    // Start the playback
    sequencer.start();
    sequencer_thread = std::thread(&AudioEngine::sequencer_thread_loop, this);
}

void AudioEngine::stop() {
    sequencer.stop();
    if (sequencer_thread.joinable()) {
        sequencer_thread.join();
    }
    synth.stop();
}

void AudioEngine::skip_seconds(float seconds) {
    bool was_playing = sequencer.is_playing();
    stop(); // Prevent any seg faults caused by race conditions and sequencer thread

    if (seconds < 0) {
        sequencer.skip_backward(-seconds);
    }
    else {
        sequencer.skip_forward(seconds);
    }

    if (was_playing) play();
}

void AudioEngine::set_track_sequence(std::size_t index) {
    if (index >= loaded_track_sequences.size()) return;
    current_track = index;
    file_has_switched = true;
}

void AudioEngine::set_global_volume(float volume) {
    synth.set_global_volume(volume);
}

void AudioEngine::soft_reset() {
    stop();
    sequencer.reset();
    synth.reset_state();
}

bool AudioEngine::is_playing() const {
    return sequencer.is_playing();
}

float AudioEngine::get_track_sequence_current_time_seconds() const {
    if (current_track < 0 || current_track >= loaded_track_sequences.size()) return 0.0f;
    return sequencer.get_current_time_seconds();
}

float AudioEngine::get_track_sequence_length_seconds() const {
    if (current_track < 0 || current_track >= loaded_track_sequences.size()) return 0.0f;
    return sequencer.get_total_duration_seconds();
}