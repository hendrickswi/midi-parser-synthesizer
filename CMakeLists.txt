cmake_minimum_required(VERSION 4.1)
project(MIDI_ParserSynthesizer)

set(CMAKE_CXX_STANDARD 20)

add_executable(MIDI_ParserSynthesizer main.cpp
        MidiParser.cpp
        MidiParser.h
        Containers/File.cpp
        Containers/File.h
        Containers/Track.cpp
        Containers/Track.h
        TrackEvents/Note.h
        TrackEvents/MidiEvent.h
        Containers/TrackSequence.cpp
        Containers/TrackSequence.h
        TrackEvents/SysexEvent.h
        TrackEvents/MetaEvent.h)
