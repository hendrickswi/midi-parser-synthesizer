cmake_minimum_required(VERSION 4.1)
project(MIDI_ParserSynthesizer)

set(CMAKE_CXX_STANDARD 20)

add_executable(MIDI_ParserSynthesizer main.cpp
        MidiParser.cpp
        MidiParser.h
        File.cpp
        File.h
        Track.cpp
        Track.h
        Note.h
        MidiEvent.h
        TrackSequence.cpp
        TrackSequence.h)
