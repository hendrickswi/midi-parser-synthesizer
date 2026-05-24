#ifndef MIDI_PARSERSYNTHESIZER_AUDIOENGINE_H
#define MIDI_PARSERSYNTHESIZER_AUDIOENGINE_H
#include <RtAudio.h>
#include <thread>

#include "Parser/MidiParser.h"
#include "Sequencer/MidiSequencer.h"

class AudioEngine {
private:
    MidiParser parser;
    MidiSequencer sequencer;
    VoiceManager synth;
    RtAudio rt_audio;

    std::vector<TrackSequence> loaded_track_sequences;
    std::vector<std::string> loaded_file_names;
    std::size_t current_track;
    bool file_has_switched;

    std::thread sequencer_thread;

    void init(float sample_rate = 44100.0f, unsigned int num_channels = 1);

    // RtAudio mandated callback function
    static int audio_callback(void *output_buffer, void *input_buffer, unsigned int num_frames, double stream_time,
        RtAudioStreamStatus status, void *user_data);

    // The continuous loop run by this->sequencer_thread
    void sequencer_thread_loop();

public:
    AudioEngine();
    AudioEngine(float sample_rate, unsigned int num_channels, float global_volume);
    AudioEngine(const AudioEngine& other) = delete;
    ~AudioEngine();

    bool load_midi_file(const std::string& file_path);
    [[nodiscard]] const std::vector<std::string>& get_loaded_file_names() const;
    [[nodiscard]] std::size_t get_current_track_sequence_index() const;

    void play();
    void stop();
    void skip_seconds(float seconds);
    void set_track_sequence(std::size_t index);
    void set_global_volume(float volume);
    void soft_reset();

    [[nodiscard]] bool is_playing() const;
    [[nodiscard]] float get_track_sequence_current_time_seconds() const;
    [[nodiscard]] float get_track_sequence_length_seconds() const;
};


#endif //MIDI_PARSERSYNTHESIZER_AUDIOENGINE_H