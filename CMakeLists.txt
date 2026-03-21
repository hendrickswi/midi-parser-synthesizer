cmake_minimum_required(VERSION 4.1)
project(MIDI_ParserSynthesizer)

set(CMAKE_CXX_STANDARD 20)

add_executable(MIDI_ParserSynthesizer main.cpp
        MidiParser.cpp
        MidiParser.h
        MidiFile.cpp
        MidiFile.h)
