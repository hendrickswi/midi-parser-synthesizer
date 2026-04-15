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
)
