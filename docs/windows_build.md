# Windows Build Notes

# Dependencies
It is assumed that you have git, cmake and mingw installed and that the midi-cube repository was cloned into a folder.
If any mingw packages are missing install them using the ming installation manager (e.g. mingw32-make)

MingW Download: https://nuwen.net/mingw.html

# Install libraries
## RtAudio
Download: https://github.com/thestk/rtaudio/releases/tag/5.1.0
Tutorial: https://stackoverflow.com/questions/40193398/how-to-compile-rtaudio-on-windows-with-mingw
Notes:
* Including math.h is not necessary
* Choose "MinGW Makefiles" when configuring
* Select buildtype release
* Set RTAUDIO_ASIO to true and RTAUDIO_WASAPI to false
* Then execute mingw32-cmake install (as admin)

Add newlines in share\rtaudio\RtAudioConfig.cmake after each include

## RtAudio
Download: https://github.com/thestk/rtmidi/releases/tag/4.0.0
Same as above

Add newlines in share\rtmidi\RtMidiConfig.cmake after each include

## LibSndFile
Download (Win32 Build): https://github.com/libsndfile/libsndfile/releases/tag/v1.0.31
Extract into your SDKs folder and name the folder libsndfile

## SFML
Download: https://www.sfml-dev.org/download/sfml/2.5.1/
Extract into your SDKs and name the folder SFML

## Boost
Download: https://sourceforge.net/projects/boost/files/boost-binaries/1.77.0/
Install
Move Boost to desired location

cd /C/local/boost_1_77_0/lib32-msvc-11.0
mkdir pdb
mv *.pdb pdb

Set env variables:
BOOST_INCLUDEDIR=E:\SDKs\boost_1_77_0
BOOST_LIBRARYDIR=E:\SDKs\boost_1_77_0\lib32-msvc-11.0
BOOST_ROOT=E:\SDKs\boost_1_77_0\boost


## Compile MidiCube
Go into the clone repository
