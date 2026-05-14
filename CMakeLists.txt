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
        Parser/MidiParser.cpp
        Parser/MidiParser.h
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
        Sequencer/MidiSequencer.cpp
        Sequencer/MidiSequencer.h
        Sequencer/TrackIndices.h
        Sequencer/ActiveNote.h
        DirectoryManipulator.cpp
        DirectoryManipulator.h
        "Synthesizer/Oscillators/Base Implementations/Oscillator.h"
        Synthesizer/Voices/Voice.cpp
        Synthesizer/Voices/Voice.h
        Synthesizer/VoiceManager.cpp
        Synthesizer/VoiceManager.h
        "Synthesizer/Oscillators/Base Implementations/SineOscillator.cpp"
        "Synthesizer/Oscillators/Base Implementations/SineOscillator.h"
        "Synthesizer/Oscillators/Base Implementations/SawtoothOscillator.cpp"
        "Synthesizer/Oscillators/Base Implementations/SawtoothOscillator.h"
        "Synthesizer/Oscillators/Base Implementations/SquareOscillator.cpp"
        "Synthesizer/Oscillators/Base Implementations/SquareOscillator.h"
        Synthesizer/Envelopes/Envelope.h
        Synthesizer/Envelopes/Decorators/TremoloEnvelope.cpp
        Synthesizer/Envelopes/Decorators/TremoloEnvelope.h
        "Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.cpp"
        "Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
        Synthesizer/Envelopes/Decorators/EnvelopeDecorator.cpp
        Synthesizer/Envelopes/Decorators/EnvelopeDecorator.h
        "Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelopeState.h"
        "Synthesizer/Oscillators/Base Implementations/Oscillator.cpp"
        Synthesizer/Oscillators/Decorators/OscillatorDecorator.cpp
        Synthesizer/Oscillators/Decorators/OscillatorDecorator.h
        Synthesizer/Oscillators/Decorators/VibratoOscillator.cpp
        Synthesizer/Oscillators/Decorators/VibratoOscillator.h
        EventTypeEnums/ContinuousControllers.h
        "Synthesizer/Oscillators/Base Implementations/TriangleOscillator.cpp"
        "Synthesizer/Oscillators/Base Implementations/TriangleOscillator.h"
        "Synthesizer/Oscillators/Base Implementations/NoiseOscillator.cpp"
        "Synthesizer/Oscillators/Base Implementations/NoiseOscillator.h"
        "Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelope.cpp"
        "Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelope.h"
        "Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelopeState.h"
        Synthesizer/InstrumentRegistry.cpp
        Synthesizer/InstrumentRegistry.h
        AudioEngine.cpp
        AudioEngine.h
        Qt/MainWindow.cpp
        Qt/MainWindow.h
        "Synthesizer/Oscillators/Base Implementations/SampleOscillator.cpp"
        "Synthesizer/Oscillators/Base Implementations/SampleOscillator.h"
        "Synthesizer/Oscillators/Base Implementations/AlgorithmicOscillator.cpp"
        "Synthesizer/Oscillators/Base Implementations/AlgorithmicOscillator.h"
        "Synthesizer/Sample Loading/dr_wav.h"
        "Synthesizer/Sample Loading/SampleLoader.cpp"
        "Synthesizer/Sample Loading/SampleLoader.h"
)

target_link_libraries(MIDI_ParserSynthesizer PRIVATE ${RTAUDIO_LIBRARIES} Qt6::Widgets)
target_include_directories(MIDI_ParserSynthesizer PRIVATE ${RTAUDIO_INCLUDE_DIRS})


