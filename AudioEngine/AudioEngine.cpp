#include "AudioEngine.h"
#include "../EventTypeEnums/MidiEventType.h"

#include <iostream>
#include <set>

void AudioEngine::sequencer_thread_loop() {
    while (sequencer.is_playing()) {
        uint64_t current_micros = sample_count_to_microseconds(global_sample_count.load(std::memory_order_relaxed), active_sample_rate);
        sequencer.update(current_micros + LOOK_AHEAD_MICROS);
        std::this_thread::sleep_for(std::chrono::microseconds(250));
    }
}

void AudioEngine::watchdog_thread_loop() {
    uint64_t last_sample_count = 0;
    uint64_t last_underrun_count = 0;
    unsigned int consecutive_underrun_count = 0;
    auto last_progress_time = std::chrono::steady_clock::now();

    while (watchdog_thread_active.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (is_playing()) {
            uint64_t current_sample_count = global_sample_count.load(std::memory_order_relaxed);
            uint64_t current_underrun_count = underrun_count.load(std::memory_order_relaxed);

            // Buffer underflow detection
            if (current_underrun_count > last_underrun_count) {
                consecutive_underrun_count += (current_underrun_count - last_underrun_count);
                last_underrun_count = current_underrun_count;

                if (consecutive_underrun_count > 0) {
                    std::cerr << "WARNING: Audio buffer underflow detected" << std::endl;
                }
            }
            else if (consecutive_underrun_count > 0) {
                consecutive_underrun_count--;
            }

            // Stream freeze detection
            if (current_sample_count != last_sample_count) {
                last_sample_count = current_sample_count;
                last_progress_time = std::chrono::steady_clock::now();
            }
            else {
                auto now = std::chrono::steady_clock::now();
                auto time_since_last_progress = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_progress_time).count();

                if (time_since_last_progress > 3000) {
                    std::cerr << "WARNING: Audio stream has frozen for " << time_since_last_progress << " ms" << std::endl;

                    // Attempt to recover
                    on_device_disconnect();

                    last_underrun_count = 0;
                    last_sample_count = global_sample_count.load(std::memory_order_relaxed);
                    consecutive_underrun_count = 0;
                    last_progress_time = std::chrono::steady_clock::now();
                }
            }
        }
        else {
            last_sample_count = global_sample_count.load(std::memory_order_relaxed);
            last_progress_time = std::chrono::steady_clock::now();
        }
    }
}

float AudioEngine::resolve_hardware_sample_rate(float fallback_sample_rate) {
    try {
        RtAudio rtaudio;
        if (rtaudio.getDeviceCount() > 0) {
            unsigned int device = rtaudio.getDefaultOutputDevice();
            RtAudio::DeviceInfo info = rtaudio.getDeviceInfo(device);
            if (info.preferredSampleRate > 0) {
                return static_cast<float>(info.preferredSampleRate);
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << "Warning: Failed to resolve hardware sample rate. "
                     "Falling back to the fallback rate of " << fallback_sample_rate << std::endl;
    }
    return fallback_sample_rate;
}

unsigned int AudioEngine::resolve_hardware_num_channels(unsigned int fallback_num_channels) {
    try {
        RtAudio rtaudio;
        if (rtaudio.getDeviceCount() > 0) {
            unsigned int device = rtaudio.getDefaultOutputDevice();
            RtAudio::DeviceInfo info = rtaudio.getDeviceInfo(device);
            if (info.outputChannels > 0) {
                return info.outputChannels;
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << "Warning: Failed to resolve hardware number of output channels. "
                     "Falling back to the fallback number of channels: " << fallback_num_channels << std::endl;
    }
    return fallback_num_channels;
}

bool AudioEngine::is_high_priority_command(SynthesizerCommandType type) {
    return type == SynthesizerCommandType::NOTE_ON ||
        type == SynthesizerCommandType::NOTE_OFF ||
        type == SynthesizerCommandType::SET_CHANNEL_PATCH ||
        type == SynthesizerCommandType::STOP_ALL_VOICES;
}

uint64_t AudioEngine::sample_count_to_microseconds(uint64_t sample_count, float sample_rate) {
    return static_cast<uint64_t>((static_cast<double>(sample_count) * 1000000.0) / static_cast<double>(sample_rate));
}

uint64_t AudioEngine::microseconds_to_sample_count(uint64_t microseconds, float sample_rate) {
    return static_cast<uint64_t>(static_cast<double>(microseconds) * static_cast<double>(sample_rate) / 1000000.0);
}

int AudioEngine::audio_callback(void *output_buffer, void *input_buffer, unsigned int num_frames, double stream_time,
        RtAudioStreamStatus status, void *user_data) {
    auto engine = static_cast<AudioEngine*>(user_data);

    // Do flush logic if told to do so
    if (engine->flush_command_queue_flag.load(std::memory_order_relaxed)) {
        engine->synth.clear_command_queue();
        engine->flush_command_queue_flag.store(false, std::memory_order_relaxed);
    }

    // Safety check for length
    if (num_frames > engine->mono_buffer.size()) {
        std::cerr << "Warning: Audio buffer size mismatch. " << num_frames << " frames requested, " << engine->mono_buffer.size() << " available." << std::endl;
        engine->mono_buffer.resize(num_frames);
    }
    auto buffer_to_use = engine->mono_buffer.data();

    std::fill(buffer_to_use, buffer_to_use + num_frames, 0.0f);

    unsigned int current_frame = 0;
    uint64_t block_end_sample = engine->global_sample_count + num_frames;
    SynthesizerCommand command;
    while (current_frame < num_frames) {
        uint64_t current_absolute_sample = engine->global_sample_count + current_frame;
        while (engine->synth.peek_from_command_queue(command)) {
            bool is_due_now = command.absolute_sample <= current_absolute_sample;
            bool is_high_priority = is_high_priority_command(command.type);
            bool is_due_this_block = command.absolute_sample < block_end_sample;

            if (is_due_now || (!is_high_priority && is_due_this_block)) {
                engine->synth.pop_from_command_queue(command);
                engine->execute_command(command);
            }
            else {
                break;
            }
        }

        unsigned int frames_to_process = num_frames - current_frame;
        if (engine->synth.peek_from_command_queue(command) && command.absolute_sample < block_end_sample) {
            frames_to_process = static_cast<unsigned int>(command.absolute_sample - current_absolute_sample);
        }

        if (frames_to_process > BUFFER_SIZE) {
            frames_to_process = BUFFER_SIZE;
        }

        if (frames_to_process < 32 && (current_frame + 32 <= num_frames)) {
            frames_to_process = 32;
        }

        if (frames_to_process > 0) {
            // Pointer arithmetic to only give the part of the buffer that needs to be processed
            engine->synth.process_audio_buffer(buffer_to_use + current_frame, frames_to_process);
            current_frame += frames_to_process;
        }
    }

    engine->global_sample_count += num_frames;
    interleave(buffer_to_use, num_frames, static_cast<float*>(output_buffer), num_frames * engine->num_channels);

    if (status & RTAUDIO_OUTPUT_UNDERFLOW) {
        engine->underrun_count.fetch_add(1, std::memory_order_relaxed);
    }

    return 0;
}

void AudioEngine::interleave(const float* mono_buffer, unsigned int mono_num_frames, float* interleaved_buffer, unsigned int interleaved_num_frames) {
    unsigned int channel_ratio = interleaved_num_frames / mono_num_frames;

    for (unsigned int i = 0; i < mono_num_frames; i++) {
        for (unsigned int j = 0; j < channel_ratio; j++) {
            interleaved_buffer[i * channel_ratio + j] = mono_buffer[i];
        }
    }
}

void AudioEngine::execute_command(const SynthesizerCommand& command) {
    switch (command.type) {
        case SynthesizerCommandType::NOTE_ON : {
            synth.note_on(command.channel, command.data1, command.data2);
            break;
        }
        case SynthesizerCommandType::NOTE_OFF : {
            synth.note_off(command.channel, command.data1);
            break;
        }
        case SynthesizerCommandType::SET_CHANNEL_PATCH : {
            synth.set_channel_patch(command.channel, command.data1);
            break;
        }
        case SynthesizerCommandType::SET_CHANNEL_PITCH_BEND : {
            const uint16_t value = (command.data1 & 0x7F) | ((command.data2 & 0x7F) << 7);
            synth.set_channel_pitch_bend(command.channel, value);
            break;
        }
        case SynthesizerCommandType::SET_CHANNEL_PRESSURE : {
            synth.set_channel_pressure(command.channel, command.data1);
            break;
        }
        case SynthesizerCommandType::SET_CONTROL_CHANGE : {
            synth.set_channel_cc(command.channel, command.data1, command.data2);
            break;
        }
        case SynthesizerCommandType::STOP_ALL_VOICES : {
            synth.stop();
            break;
        }
        default : {
            std::cerr << "Warning: Unhandled synthesizer command type " << static_cast<int>(command.type) << std::endl;
            break;
        }
    }
}

void AudioEngine::open_audio_stream() {
    if (rt_audio.getDeviceCount() < 1) {
        std::cerr << "Warning: No audio devices found" << std::endl;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = rt_audio.getDefaultOutputDevice();
    parameters.nChannels = num_channels;
    parameters.firstChannel = 0;
    unsigned int buffer_size = BUFFER_SIZE;

    try {
        rt_audio.openStream(
            &parameters,
            nullptr,
            RTAUDIO_FLOAT32,
            static_cast<unsigned int>(active_sample_rate),
            &buffer_size,
            &audio_callback,
            this,
            nullptr
        );

        mono_buffer.resize(buffer_size);
        rt_audio.startStream();
        auto current_api = rt_audio.getCurrentApi();
        std::cout << "Audio engine now running at " << active_sample_rate << " Hz, "
            << num_channels << " channels, " << buffer_size << " frames, "
            "using " << RtAudio::getApiDisplayName(current_api) << std::endl << std::endl;
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("AudioEngine::open_audio_stream() failed during RtAudio stream opening/starting. Error:") += e.what());
    }
}

void AudioEngine::close_audio_stream() {
    stop();
    if (!rt_audio.isStreamOpen()) return;
    if (rt_audio.isStreamRunning()) rt_audio.stopStream();
    rt_audio.closeStream();
}

void AudioEngine::on_device_disconnect() {
    std::cout << "INFO: Audio device disconnected. Resetting audio engine to use new default device." << std::endl;

    bool was_playing = sequencer.is_playing();
    close_audio_stream();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    active_sample_rate = resolve_hardware_sample_rate(active_sample_rate);
    sequencer.set_sample_rate(active_sample_rate);
    synth.set_sample_rate(active_sample_rate);
    num_channels = resolve_hardware_num_channels(num_channels);
    open_audio_stream();

    if (was_playing) play();
}

AudioEngine::AudioEngine(float fallback_sample_rate, float global_volume)
    : parser(),
    sequencer(resolve_hardware_sample_rate(fallback_sample_rate)),
    synth(resolve_hardware_sample_rate(fallback_sample_rate)) {

    sequencer.set_synthesizer(&synth);
    underrun_count.store(0, std::memory_order_relaxed);

    active_sample_rate = resolve_hardware_sample_rate(fallback_sample_rate);
    sequencer.set_sample_rate(active_sample_rate);
    synth.set_sample_rate(active_sample_rate);
    synth.set_global_volume(global_volume);

    num_channels = resolve_hardware_num_channels(1);
    global_sample_count.store(0, std::memory_order_relaxed);
    flush_command_queue_flag.store(false, std::memory_order_relaxed);
    loaded_track_sequences = std::vector<TrackSequence>();
    loaded_file_names = std::vector<std::string>();
    current_track = -1;
    file_has_switched = false;

    open_audio_stream();

    watchdog_thread = std::thread(&AudioEngine::watchdog_thread_loop, this);
    watchdog_thread_active.store(true, std::memory_order_relaxed);
}

AudioEngine::~AudioEngine() {
    watchdog_thread_active.store(false, std::memory_order_relaxed);
    if (watchdog_thread.joinable()) {
        watchdog_thread.join();
    }

    close_audio_stream();

    if (watchdog_thread.joinable()) {
        watchdog_thread.join();
    }
    watchdog_thread_active.store(false, std::memory_order_relaxed);
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

bool AudioEngine::get_peak_amplitude_normalization() const {
    return synth.get_peak_amplitude_normalization();
}

void AudioEngine::play() {
    if (current_track >= loaded_track_sequences.size() || sequencer.is_playing()) return;

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
        sequencer.set_track_sequence(&loaded_track_sequences[current_track]);
        sequencer.set_synthesizer(&synth);
    }

    // Start the playback
    sequencer.start(sample_count_to_microseconds(
        global_sample_count.load(std::memory_order_relaxed),
        active_sample_rate
        )
    );
    sequencer_thread = std::thread(&AudioEngine::sequencer_thread_loop, this);
}

void AudioEngine::stop() {
    sequencer.stop();
    if (sequencer_thread.joinable()) {
        sequencer_thread.join();
    }

    if (rt_audio.isStreamRunning()) {
        // Delegate command queue flush logic to the audio thread
        flush_command_queue_flag.store(true, std::memory_order_release);

        // Wait until flush is done
        while (flush_command_queue_flag.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
    else {
        // Audio thread is not active, so can just flush from this thread
        synth.clear_command_queue();
    }

    synth.stop();

    uint64_t current_micros = sample_count_to_microseconds(global_sample_count.load(std::memory_order_relaxed), active_sample_rate);
    uint64_t sequencer_current_micros = sequencer.get_total_elapsed_micros();

    if (sequencer_current_micros > current_micros) {
        float overshot_seconds = static_cast<float>(sequencer_current_micros - current_micros) / 1000000.0;
        skip_seconds(-overshot_seconds);
    }
}

void AudioEngine::skip_seconds(float seconds) {
    bool was_playing = sequencer.is_playing();
    stop(); // Prevent thread collisions

    if (seconds < 0) {
        synth.reset_state();
        sequencer.skip_backward(-seconds);
    }
    else {
        sequencer.skip_forward(seconds);
    }

    uint64_t new_micros = sequencer.get_total_elapsed_micros();
    this->global_sample_count = sample_count_to_microseconds(new_micros, active_sample_rate);

    if (was_playing) play();
}

void AudioEngine::set_track_sequence(std::size_t index) {
    if (index >= loaded_track_sequences.size()) return;
    current_track = index;
    file_has_switched = true;
    global_sample_count.store(0, std::memory_order_relaxed);
    underrun_count.store(0, std::memory_order_relaxed);
}

void AudioEngine::set_global_volume(float volume) {
    synth.set_global_volume(volume);
}

void AudioEngine::set_peak_amplitude_normalization(bool enabled) {
    synth.set_peak_amplitude_normalization(enabled);
}

void AudioEngine::soft_reset() {
    stop();

    if (current_track >= loaded_track_sequences.size()) {
        return;
    }

    // Soft reset sequencer
    sequencer.reset();
    sequencer.set_track_sequence(&loaded_track_sequences[current_track]);
    sequencer.set_synthesizer(&synth);

    // Soft reset synthesizer
    synth.reset_state();
    global_sample_count.store(0, std::memory_order_relaxed);
    underrun_count.store(0, std::memory_order_relaxed);
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

uint64_t AudioEngine::get_underrun_count() const {
    return underrun_count;
}
