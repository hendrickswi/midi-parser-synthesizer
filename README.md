# MIDI Synthesizer

A robust, Windows and Linux C++ MIDI audio engine integrating a custom parser, sequencer, and synthesizer. Features 
realistic instrument playback via .wav file sample playback for General MIDI patches.<br><br>

## Features

- User-friendly GUI with buttons and sliders that allow the user to manipulate the playback. For example,

  - Play/pause
  - Skip to next/previous track
  - Volume slider
  - Playback speed slider
  - Repeat and shuffle toggles
  - Seek slider
  
 - Easy addition of .mid files by single file or bulk directory.

 - Support for Format 0 and 1 .mid file playback.

 - Multi-threaded, highly accurate and realistic playback.

 - General MIDI Patches Mapped:

   - Melodic: 0-95, with applicable instruments having .wav file sample playback.
   - Drum: 35-59, 81 with applicable instruments having .wav file sample playback.

 - Resilient runtime, allowing for detection of audio buffer underruns and audio stream recovery in the event of hardware disconnection.

 - x64 Windows and x64 Linux support. Unfortunately, Mac support cannot be tested (I do not own a Mac).<br><br>

## Getting Started

### x64 Windows

 1. Ensure you have the the Microsoft Visual C++ Runtime libraries
 
    - You can find the redistributable here: https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170
    - Generally, download the latest supported version.
 
 2. Download this release and extract it

 3. Run the pre-compiled binary (MIDI_ParserSynthesizer.exe) in the extracted directory!<br><br>

### x64 Linux

1. Download this release and extract it

2. Make the included pre-compiled binary (MIDI_ParserSynthesizer) executable.

    - This can be done by cd'ing into the download directory and running `chmod +x MIDI_ParserSynthesizer`
    - Alternatively, right click the pre-compiled binary in the file explorer, click on properties, and check off the "Executable as Program" toggle.

 3. Run the pre-compiled binary!
 
    - In the same terminal window, simply type `./MIDI_ParserSynthesizer`
    - Alternatively, double click the pre-compiled binary (MIDI_ParserSynthesizer) in the file explorer.<br><br>

## Usage

This software can generally be used to play any Format 0 or 1 MIDI (.mid) file, and the best playback will come from MIDI files that 
adhere to the MIDI 1.0 specification.

<br>To get started, launch the program, and add your MIDI file(s) to the program by clicking on 'File', then
'Add File' or 'Add Directory' in the top bar. Select any MIDI file(s) you want to synthesize with this program in the window that pops up, and press
the 'Open' button in the window.

<br>From here, the program acts much like a general purpose media player, with various controls such as the ability to adjust volume, or skip to the
next track.
