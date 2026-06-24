cmake_minimum_required(VERSION 4.1)
project(MIDI_ParserSynthesizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt auto compilers
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(CMAKE_HOST_WIN32)
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/qt/6.7.0/msvc2019_64")
elseif(CMAKE_HOST_UNIX AND NOT CMAKE_HOST_APPLE)
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/qt/6.7.0/gcc_64")
endif()

# relies on the user to install qt6.70 via aqtinstall
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# vcpkg ensures this is available
find_package(RtAudio CONFIG REQUIRED)

include(FetchContent)

# JSON Parser fetch
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG develop
)
FetchContent_MakeAvailable(json)

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
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Base Oscillator/Oscillator.h"
        AudioEngine/Synthesizer/Voices/Voice.cpp
        AudioEngine/Synthesizer/Voices/Voice.h
        AudioEngine/Synthesizer/VoiceManager.cpp
        AudioEngine/Synthesizer/VoiceManager.h
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Algorithmic/Sine/SineOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Algorithmic/Sine/SineOscillator.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Algorithmic/Sawtooth/SawtoothOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Algorithmic/Sawtooth/SawtoothOscillator.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Algorithmic/Square/SquareOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Algorithmic/Square/SquareOscillator.h"
        AudioEngine/Synthesizer/Envelopes/Envelope.h
        AudioEngine/Synthesizer/Envelopes/Decorators/TremoloEnvelope.cpp
        AudioEngine/Synthesizer/Envelopes/Decorators/TremoloEnvelope.h
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.cpp"
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
        AudioEngine/Synthesizer/Envelopes/Decorators/EnvelopeDecorator.cpp
        AudioEngine/Synthesizer/Envelopes/Decorators/EnvelopeDecorator.h
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelopeState.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Base Oscillator/Oscillator.cpp"
        AudioEngine/Synthesizer/Oscillators/Decorators/OscillatorDecorator.cpp
        AudioEngine/Synthesizer/Oscillators/Decorators/OscillatorDecorator.h
        AudioEngine/Synthesizer/Oscillators/Decorators/VibratoOscillator.cpp
        AudioEngine/Synthesizer/Oscillators/Decorators/VibratoOscillator.h
        EventTypeEnums/ContinuousControllers.h
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Algorithmic/Triangle/TriangleOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Algorithmic/Triangle/TriangleOscillator.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Noise/NoiseOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Noise/NoiseOscillator.h"
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelope.cpp"
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelope.h"
        "AudioEngine/Synthesizer/Envelopes/Base Implementations/ADR/ADREnvelopeState.h"
        "AudioEngine/Patch Configuration/InstrumentRegistry.cpp"
        "AudioEngine/Patch Configuration/InstrumentRegistry.h"
        AudioEngine/AudioEngine.cpp
        AudioEngine/AudioEngine.h
        Qt/MainWindow.cpp
        Qt/MainWindow.h
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Sample/SampleOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Sample/SampleOscillator.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Algorithmic/AlgorithmicOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Algorithmic/AlgorithmicOscillator.h"
        "AudioEngine/Synthesizer/Sample Loading/dr_wav.h"
        "AudioEngine/Synthesizer/Sample Loading/SampleLoader.cpp"
        "AudioEngine/Synthesizer/Sample Loading/SampleLoader.h"
        "AudioEngine/Synthesizer/Sample Loading/Sample.h"
        "AudioEngine/Patch Configuration/EnvelopeParsingType.h"
        resources.qrc
        Qt/PlaybackController.cpp
        Qt/PlaybackController.h
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Composite/CompositeOscillator.cpp"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Composite/CompositeOscillator.h"
        "AudioEngine/Synthesizer/Oscillators/Base Implementations/Composite/CompositeOscillatorNode.h"
        "AudioEngine/Synthesizer/Sample Loading/RawAudioData.h"
        AudioEngine/Parser/UnfinishedNote.h
        AudioEngine/Synthesizer/LockFreeQueue.h
        AudioEngine/Synthesizer/SynthesizerCommand.h
        "AudioEngine/Patch Configuration/PatchDefinition.h"
        "AudioEngine/Synthesizer/Lookup Tables/SineLookupTable.h"
        "AudioEngine/Patch Configuration/OscillatorParsingType.h"
        AudioEngine/PlaylistNavigator.cpp
        AudioEngine/PlaylistNavigator.h
        AudioEngine/Synthesizer/Oscillators/Filters/ChamberlinSVF.cpp
        AudioEngine/Synthesizer/Oscillators/Filters/ChamberlinSVF.h
        AudioEngine/Synthesizer/Oscillators/Decorators/LowpassFilterOscillator.cpp
        AudioEngine/Synthesizer/Oscillators/Decorators/LowpassFilterOscillator.h
        AudioEngine/Synthesizer/Oscillators/Decorators/BandpassFilterOscillator.cpp
        AudioEngine/Synthesizer/Oscillators/Decorators/BandpassFilterOscillator.h
        AudioEngine/Synthesizer/Oscillators/Decorators/HighpassFilterOscillator.cpp
        AudioEngine/Synthesizer/Oscillators/Decorators/HighpassFilterOscillator.h
)

target_link_libraries(MIDI_ParserSynthesizer PRIVATE
        Qt6::Widgets
        RtAudio::rtaudio
        nlohmann_json::nlohmann_json)

# Workaround for GCC 14+ emitting duplicate _Float16 templates with Qt 6.7.0
if(UNIX AND NOT APPLE)
    target_link_options(MIDI_ParserSynthesizer PRIVATE "-Wl,--allow-multiple-definition")
endif()

# Platform-specific audio backend link requirements
if(APPLE)
    target_link_libraries(MIDI_ParserSynthesizer PRIVATE
            "-framework CoreAudio"
            "-framework CoreFoundation"
            "-framework AudioToolbox"
    )
elseif(WIN32)
    target_link_libraries(MIDI_ParserSynthesizer PRIVATE ksuser mfplat mfuuid wmcodecdspuuid)
endif()

# Populate build directory with the necessary .dll files to prevent immediate crash
if(WIN32)
    get_target_property(_qt_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qt_qmake_executable}" DIRECTORY)

    add_custom_command(TARGET MIDI_ParserSynthesizer POST_BUILD
            COMMAND "${_qt_bin_dir}/windeployqt.exe"
            --no-translations
            --no-opengl-sw
            "$<TARGET_FILE:MIDI_ParserSynthesizer>"
            COMMENT "Running windeployqt to populate DLLs..."
    )
endif()

# Copy assets (wav samples and json configs) to the output directory
set(ASSETS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
set(ASSETS_BIN_DIR "$<TARGET_FILE_DIR:MIDI_ParserSynthesizer>/Assets")
add_custom_target(CopyRuntimeAssets
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_SRC_DIR}" "${ASSETS_BIN_DIR}"
        COMMENT "Syncing updated runtime assets (wav/json) to the build directory..."
)
add_dependencies(MIDI_ParserSynthesizer CopyRuntimeAssets)


