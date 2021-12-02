# Windows Build Notes

# Dependencies
It is assumed that you have git, cmake and mingw installed and that the midi-cube repository was cloned into a folder.
If any mingw packages are missing install them using the ming installation manager (e.g. mingw32-make)

MingW Download: https://nuwen.net/mingw.html

# Install libraries
## RtAudio
* Download from: https://github.com/thestk/rtaudio/releases/tag/5.1.0
* Installation Tutorial: https://stackoverflow.com/questions/40193398/how-to-compile-rtaudio-on-windows-with-mingw
    * Including math.h is not necessary
    * Choose "MinGW Makefiles" when configuring
    * Select buildtype release
    * Set RTAUDIO_DS to true and RTAUDIO_WASAPI, RTAUDIO_DS, RTAUDIO_ASIO to false
    * Then execute mingw32-cmake install (as admin)
* Add newlines in share\rtaudio\RtAudioConfig.cmake after each include

## RtAudio
* Download from : https://github.com/thestk/rtmidi/releases/tag/4.0.0
* Same as above
* Add newlines in share\rtmidi\RtMidiConfig.cmake after each include

## LibSndFile
* Download from (Win64 Build): https://github.com/libsndfile/libsndfile/releases/tag/v1.0.31
* Extract into your SDKs folder and name the folder libsndfile

## SFML
* Download from (Win64 Build): https://www.sfml-dev.org/download/sfml/2.5.1/
* Extract into your SDKs and name the folder SFML

## Boost
* Download from: https://sourceforge.net/projects/boost/files/boost-binaries/1.77.0/
* Install with installer
* Execute the following commands
    cd /C/local/boost_1_77_0/lib32-msvc-11.0
    mkdir pdb
    mv *.pdb pdb
* Move Boost to desired location

* Set env variables:
    BOOST_INCLUDEDIR=E:\SDKs\boost_1_77_0
    BOOST_LIBRARYDIR=E:\SDKs\boost_1_77_0\lib32-msvc-11.0
    BOOST_ROOT=E:\SDKs\boost_1_77_0\boost


## Compile MidiCube
* Go into the clone repository
* Open CMake GUI and select project folder and create a build folder
* Set CMAKE_PREFIX_PATH to E:\SDKs\SFML\lib\cmake\SFML;E:\SDKs\rtmidi\share\rtmidi;E:\SDKs\rtaudio\share\rtaudio;E:\SDKs\libsndfile\cmake;
* Generate MingW Makefiles
* Create rtaudio and rtmidi folders in the project include folder and copy the headers of the libraries into them
* Execute make in your build folder
* Copy the dll files of the libraries into the build folder (librtaudio.dll librtmidi.dll sfml-graphics-2.dll sfml-system-2.dll sfml-window-2.dll sndfile.dll)
* Copy libgcc_s_seh-1.dll, libstc++-6.dll and libwinpthread-1.dll bin/mingw64 from your Git for Windows Installation
* Copy the same DLLs into the midi-cube/lib folder
