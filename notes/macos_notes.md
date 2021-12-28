# MasOS Install Notes

## Dependencies
- Install git, xcode command line tools and homebrew
- Install sfml via homebrew
- Download rtmidi from the official website
    - Unzip
    - Create build folder and enter it
    - CMAKE_BUILD_TYPE=Release cmake ..
    - make install
- Download rtaudio from the official website
    - Unzip
    - Create build folder and enter it
    - CMAKE_BUILD_TYPE=Release cmake ..
    - make install
- Download libsndfile from the official website
	- Unzip
   	- Create build folder and enter it
    - CMAKE_BUILD_TYPE=Release cmake ..
    - make install

## Install MidiCube
- Clone the MidiCube Repository
- Create cmake Folder and copy all files in the cmake directory of libsndfile starting with "Find" into the MidiCube directory
- Create build folder and enter it
- CMAKE_BUILD_TYPE=Release cmake ..
- make -DESTDIR=/path/to/dest install
