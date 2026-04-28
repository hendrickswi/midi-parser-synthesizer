cmake_minimum_required(VERSION 4.1)
project(MIDI_ParserSynthesizer)

set(CMAKE_CXX_STANDARD 20)

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
        FilePathSanitizer.cpp
        FilePathSanitizer.h
        Synthesizer/Oscillators/Oscillator.h
        Synthesizer/Voices/Voice.cpp
        Synthesizer/Voices/Voice.h
        Synthesizer/VoiceManager.cpp
        Synthesizer/VoiceManager.h
        Synthesizer/Oscillators/SineOscillator.cpp
        Synthesizer/Oscillators/SineOscillator.h
        Synthesizer/Oscillators/SawtoothOscillator.cpp
        Synthesizer/Oscillators/SawtoothOscillator.h
        Synthesizer/Oscillators/SquareOscillator.cpp
        Synthesizer/Oscillators/SquareOscillator.h
        Synthesizer/Envelopes/Envelope.h
        Synthesizer/Envelopes/Decorators/TremoloEnvelope.cpp
        Synthesizer/Envelopes/Decorators/TremoloEnvelope.h
        "Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.cpp"
        "Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelope.h"
        Synthesizer/Envelopes/Decorators/EnvelopeDecorator.cpp
        Synthesizer/Envelopes/Decorators/EnvelopeDecorator.h
        "Synthesizer/Envelopes/Base Implementations/ADSR/ADSREnvelopeState.h"
        Synthesizer/Oscillators/Oscillator.cpp
        Synthesizer/Envelopes/Decorators/VibratoEnvelope.cpp
        Synthesizer/Envelopes/Decorators/VibratoEnvelope.h
)
