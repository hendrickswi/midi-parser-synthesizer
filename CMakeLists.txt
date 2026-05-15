cmake_minimum_required(VERSION 4.1)
project(MIDI_ParserSynthesizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt auto compilers
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

include_directories(SYSTEM /usr/include/rtaudio)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RTAUDIO REQUIRED rtaudio)

add_executable(MIDI_ParserSynthesizer main.cpp
        AudioEngine/Parser/MidiParser.cpp
        AudioEngine/Parser/MidiParser.h
        Containers/File.cpp
        Containers/File.h
        Containers/Track.cpp
        Containers/Track.h
        TrackEvents/Note.h
        TrackEvents/MidiEvent.h
        Containers/TrackSequence.cpp
        Containers/TrackSequence.h
        TrackEvents/SysexEvent.h
        TrackEvents/MetaEvent.h
        EventTypeEnums/MetaEventType.h
        EventTypeEnums/EventType.h
        EventTypeEnums/MidiEventType.h
        EventTypeEnums/SysexEventType.h
        AudioEngine/Sequencer/MidiSequencer.cpp
        AudioEngine/Sequencer/MidiSequencer.h
        AudioEngine/Sequencer/TrackIndices.h
        AudioEngine/Sequencer/ActiveNote.h
        DirectoryManipulator.cpp
        DirectoryManipulator.h
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Oscillator.h"
        AudioEngine/Synthesizer/Voices/Voice.cpp
        AudioEngine/Synthesizer/Voices/Voice.h
        AudioEngine/Synthesizer/VoiceManager.cpp
        AudioEngine/Synthesizer/VoiceManager.h
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/SineOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/SineOscillator.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/SawtoothOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/SawtoothOscillator.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/SquareOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/SquareOscillator.h"
        AudioEngine/Synthesizer/Envelopes/Envelope.h
        AudioEngine/Synthesizer/Envelopes/Decorators/TremoloEnvelope.cpp
        AudioEngine/Synthesizer/Envelopes/Decorators/TremoloEnvelope.h
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.cpp"
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
        AudioEngine/Synthesizer/Envelopes/Decorators/EnvelopeDecorator.cpp
        AudioEngine/Synthesizer/Envelopes/Decorators/EnvelopeDecorator.h
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelopeState.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Oscillator.cpp"
        AudioEngine/Synthesizer/Oscillators/Decorators/OscillatorDecorator.cpp
        AudioEngine/Synthesizer/Oscillators/Decorators/OscillatorDecorator.h
        AudioEngine/Synthesizer/Oscillators/Decorators/VibratoOscillator.cpp
        AudioEngine/Synthesizer/Oscillators/Decorators/VibratoOscillator.h
        EventTypeEnums/ContinuousControllers.h
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/TriangleOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/TriangleOscillator.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/NoiseOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/NoiseOscillator.h"
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelope.cpp"
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelope.h"
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelopeState.h"
        AudioEngine/Synthesizer/InstrumentRegistry.cpp
        AudioEngine/Synthesizer/InstrumentRegistry.h
        AudioEngine/AudioEngine.cpp
        AudioEngine/AudioEngine.h
        Qt/MainWindow.cpp
        Qt/MainWindow.h
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/SampleOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/SampleOscillator.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/AlgorithmicOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/AlgorithmicOscillator.h"
        "AudioEngine/Synthesizer/Sample Loading/dr_wav.h"
        "AudioEngine/Synthesizer/Sample Loading/SampleLoader.cpp"
        "AudioEngine/Synthesizer/Sample Loading/SampleLoader.h"
)

target_link_libraries(MIDI_ParserSynthesizer PRIVATE ${RTAUDIO_LIBRARIES} Qt6::Widgets)
target_include_directories(MIDI_ParserSynthesizer PRIVATE ${RTAUDIO_INCLUDE_DIRS})


