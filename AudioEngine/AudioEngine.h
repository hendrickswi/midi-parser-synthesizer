#ifndef MIDI_PARSERSYNTHESIZER_AUDIOENGINE_H
#define MIDI_PARSERSYNTHESIZER_AUDIOENGINE_H
#include <RtAudio.h>
#include <thread>
#include <mutex>

#include "Parser/MidiParser.h"
#include "Sequencer/MidiSequencer.h"

constexpr unsigned int BUFFER_SIZE = 1024;
constexpr uint64_t LOOK_AHEAD_MICROS = 50000;

class AudioEngine {
private:
    // The main three custom classes for midi playback
    MidiParser parser;
    MidiSequencer sequencer;
    VoiceManager synth;

    // RtAudio for OS communication
    RtAudio rt_audio;
    std::string active_device_name;

    // Audio buffer stuffs
    std::vector<float> mono_buffer;
    std::atomic<uint64_t> underrun_count;
    float active_sample_rate;
    unsigned int num_channels;
    std::atomic<uint64_t> global_sample_count;

    // Helps to avoid thread collisions
    std::atomic<bool> flush_command_queue_flag;
    std::recursive_mutex transport_mutex;

    std::vector<TrackSequence> loaded_track_sequences;
    std::vector<std::string> loaded_file_names;
    std::size_t current_track;
    bool file_has_switched;

    // The sequencer thread that schedules midi events, etc.
    std::thread sequencer_thread;
    void sequencer_thread_loop();

    // The watchdog thread that monitors performance
    std::thread watchdog_thread;
    std::atomic<bool> watchdog_thread_active;
    void watchdog_thread_loop();

    // Helper array of strings for get_instrument_names_of_current_track_sequence()
    const std::string GM_MELODIC_PATCH_NAMES[128] = {
        "Acoustic Grand Piano", "Bright Acoustic Piano", "Electric Grand Piano", "Honky-tonk Piano",
        "Electric Piano 1 (Rhodes)", "Electric Piano 2 (Chorused)", "Harpsichord", "Clavinet",

        // Chromatic Percussion (8 - 15)
        "Celesta", "Glockenspiel", "Music Box", "Vibraphone",
        "Marimba", "Xylophone", "Tubular Bells", "Dulcimer",

        // Organ (16 - 23)
        "Drawbar Organ", "Percussive Organ", "Rock Organ", "Church Organ",
        "Reed Organ", "Accordion", "Harmonica", "Tango Accordion",

        // Guitar (24 - 31)
        "Acoustic Guitar (Nylon)", "Acoustic Guitar (Steel)", "Electric Guitar (Jazz)", "Electric Guitar (Clean)",
        "Electric Guitar (Muted)", "Overdriven Guitar", "Distortion Guitar", "Guitar Harmonics",

        // Bass (32 - 39)
        "Acoustic Bass", "Electric Bass (Finger)", "Electric Bass (Pick)", "Fretless Bass",
        "Slap Bass 1", "Slap Bass 2", "Synth Bass 1", "Synth Bass 2",

        // Strings (40 - 47)
        "Violin", "Viola", "Cello", "Contrabass",
        "Tremolo Strings", "Pizzicato Strings", "Orchestral Harp", "Timpani",

        // Ensemble (48 - 55)
        "String Ensemble 1", "String Ensemble 2", "Synth Strings 1", "Synth Strings 2",
        "Choir Aahs", "Voice Oohs", "Synth Voice", "Orchestra Hit",

        // Brass (56 - 63)
        "Trumpet", "Trombone", "Tuba", "Muted Trumpet",
        "French Horn", "Brass Section", "Synth Brass 1", "Synth Brass 2",

        // Reed (64 - 71)
        "Soprano Sax", "Alto Sax", "Tenor Sax", "Baritone Sax",
        "Oboe", "English Horn", "Bassoon", "Clarinet",

        // Pipe (72 - 79)
        "Piccolo", "Flute", "Recorder", "Pan Flute",
        "Blown Bottle", "Shakuhachi", "Whistle", "Ocarina",

        // Synth Lead (80 - 87)
        "Lead 1 (Square)", "Lead 2 (Sawtooth)", "Lead 3 (Calliope)", "Lead 4 (Chiff)",
        "Lead 5 (Charang)", "Lead 6 (Voice)", "Lead 7 (Fifths)", "Lead 8 (Bass + Lead)",

        // Synth Pad (88 - 95)
        "Pad 1 (New Age)", "Pad 2 (Warm)", "Pad 3 (Polysynth)", "Pad 4 (Choir)",
        "Pad 5 (Bowed)", "Pad 6 (Metallic)", "Pad 7 (Halo)", "Pad 8 (Sweep)",

        // Synth Effects (96 - 103)
        "FX 1 (Rain)", "FX 2 (Soundtrack)", "FX 3 (Crystal)", "FX 4 (Atmosphere)",
        "FX 5 (Brightness)", "FX 6 (Goblins)", "FX 7 (Echoes)", "FX 8 (Sci-fi)",

        // Ethnic (104 - 111)
        "Sitar", "Banjo", "Shamisen", "Koto",
        "Kalimba", "Bagpipe", "Fiddle", "Shanai",

        // Percussive (112 - 119)
        "Tinkle Bell", "Agogo", "Steel Drums", "Woodblock",
        "Taiko Drum", "Melodic Tom", "Synth Drum", "Reverse Cymbal",

        // Sound Effects (120 - 127)
        "Guitar Fret Noise", "Breath Noise", "Seashore", "Bird Tweet",
        "Telephone Ring", "Helicopter", "Applause", "Gunshot"
    };

    const std::string GM_DRUM_PATCH_NAMES[128] = {
        // 0 - 34: Unmapped in GM 1.0
        "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped",
        "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped",
        "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped",
        "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped",
        "Unmapped", "Unmapped", "Unmapped",

        // 35 - 42
        "Acoustic Bass Drum", "Bass Drum 1", "Side Stick", "Acoustic Snare",
        "Hand Clap", "Electric Snare", "Low Floor Tom", "Closed Hi-Hat",

        // 43 - 50
        "High Floor Tom", "Pedal Hi-Hat", "Low Tom", "Open Hi-Hat",
        "Low-Mid Tom", "Hi-Mid Tom", "Crash Cymbal 1", "High Tom",

        // 51 - 58
        "Ride Cymbal 1", "Chinese Cymbal", "Ride Bell", "Tambourine",
        "Splash Cymbal", "Cowbell", "Crash Cymbal 2", "Vibraslap",

        // 59 - 66
        "Ride Cymbal 2", "Hi Bongo", "Low Bongo", "Mute Hi Conga",
        "Open Hi Conga", "Low Conga", "High Timbale", "Low Timbale",

        // 67 - 74
        "High Agogo", "Low Agogo", "Cabasa", "Maracas",
        "Short Whistle", "Long Whistle", "Short Guiro", "Long Guiro",

        // 75 - 82
        "Claves", "Hi Wood Block", "Low Wood Block", "Mute Cuica",
        "Open Cuica", "Mute Triangle", "Open Triangle", "Shaker",

        // 83 - 127: Unmapped in GM 1.0
        "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped",
        "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped",
        "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped",
        "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped",
        "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped",
        "Unmapped", "Unmapped", "Unmapped", "Unmapped", "Unmapped"
    };

    static float resolve_hardware_sample_rate(float fallback_sample_rate);
    static unsigned int resolve_hardware_num_channels(unsigned int fallback_num_channels);
    static bool is_high_priority_command(SynthesizerCommandType type);
    static inline uint64_t sample_count_to_microseconds(uint64_t sample_count, float sample_rate);
    static inline uint64_t microseconds_to_sample_count(uint64_t microseconds, float sample_rate);

    // RtAudio mandated callback function
    static int audio_callback(void *output_buffer, void *input_buffer, unsigned int num_frames, double stream_time,
        RtAudioStreamStatus status, void *user_data);

    // Convert mono processed samples to multiple channel audio by interleaving
    static void interleave(const float* mono_buffer, unsigned int mono_num_frames, float* interleaved_buffer, unsigned int interleaved_num_frames);

    // Helper dispatcher method for processing commands
    void execute_command(const SynthesizerCommand& command);

    // Audio stream controls
    void open_audio_stream();
    void close_audio_stream();
    void recover_stream();

public:
    explicit AudioEngine(float fallback_sample_rate = 48000.0f, float global_volume = 1.0f);
    AudioEngine(const AudioEngine& other) = delete;
    ~AudioEngine();

    bool load_midi_file(const std::string& file_path);
    [[nodiscard]] const std::vector<std::string>& get_loaded_file_names() const;
    [[nodiscard]] std::size_t get_current_track_sequence_index() const;
    [[nodiscard]] std::vector<std::string> get_instrument_names_of_current_track_sequence() const;
    [[nodiscard]] bool get_peak_amplitude_normalization() const;

    void play();
    void stop();
    void skip_seconds(float seconds);
    void set_track_sequence(std::size_t index);
    void set_global_volume(float volume);
    void set_peak_amplitude_normalization(bool enabled);
    void soft_reset();

    [[nodiscard]] bool is_playing() const;
    [[nodiscard]] float get_track_sequence_current_time_seconds() const;
    [[nodiscard]] float get_track_sequence_length_seconds() const;
    [[nodiscard]] uint64_t get_underrun_count() const;
};


#endif //MIDI_PARSERSYNTHESIZER_AUDIOENGINE_H