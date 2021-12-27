# midi-cube

A Synthesizer-Workstation written in C++, intended to both work on Single-Chip-Devices with a touch display (Raspberry Pi) and computers.

## Planned features:
* Serveral integrated virtual instruments like a B3 organ, an E-Piano-synthesizer, a sampler, drumkits, a subtractive/additive/FM-synthesizer, etc.
* MIDI-Routing between devices
* Looper, Arpeggiator, Sequencer/Drum Machine
* A touch responsive GUI
* A web interface or a desktop client that allows uploading new sounds

## Installation
On Windows the building process is quite complicated, which is why I supply Windows binaries with every released version.
On Linux the build process is pretty straighforward. MidiCube supports two build systems: Meson and CMake. The Meson build file compiles everything into one executable and compiles faster. CMake is slower but separates the project into different modules (Note: only the compilation process takes longer, MidiCube will have the same performance with both). At the moment it doesn't matter which build system you use but in future I will add a plugin API and different "hosts" (like the workstation ui you know now, a stripped down ui for use as an fx device on a raspi or a DAW plugin) where it might be from advantage to use the modular CMake build. Furthermore, CMake allows you to install MidiCube with all the relevant files using make install.
### How to install with meson on Ubuntu/Debian
1. Clone this repository
2. Make sure to install the dependencies
		
		sudo apt install meson ninja-build libsndfile-dev libsfml-dev librtaudio-dev librtmidi-dev libboost-all-dev
		
3. Build it using meson (working directory is the clone project)
		
		mkdir build
		meson ./ build
		cd build
		meson configure --buildtype release
		ninja
		
5. Run the compiled "midi-cube" file in the root folder of the project

### How to install with CMake on Ubuntu/Debian
1. Clone this repository
2. Make sure to install the dependencies
		sudo apt install cmake build-essential libsndfile-dev libsfml-dev librtaudio-dev librtmidi-dev libboost-all-dev
3. Build it using meson (working directory is the clone project)

		mkdir build
		cd build
		CMAKE_BUILD_TYPE=Release cmake ..
		make DESTDIR=/your/install/location install

5. Run the compiled "midi-cube-workstation" file in the installation folder


## Used libraries
* RtAudio licensed under the RtAudio License which is pretty similar to the MIT License (https://github.com/thestk/rtaudio https://github.com/thestk/rtaudio/blob/master/LICENSE)
* RtMidi licensed under the RtMidi license which is pretty similar to the MIT license (https://github.com/thestk/rtmidi https://github.com/thestk/rtmidi/blob/master/LICENSE)
* libsndfile licensed under LGPL (https://github.com/erikd/libsndfile/commits/master https://github.com/erikd/libsndfile/blob/master/COPYING)
* FluidSynth API for playing soundfonts licensed under LGPL-2.1 (https://github.com/FluidSynth/fluidsynth https://github.com/FluidSynth/fluidsynth/blob/master/LICENSE)
* Boost licensed under the Boost Software License (https://github.com/boostorg/boost https://github.com/boostorg/boost/blob/master/LICENSE_1_0.txt)
* SFML licensed under the zlib/png license (https://github.com/SFML/SFML https://github.com/SFML/SFML/blob/master/license.md)

## Used Assets
* Liberation font licensed under the Open Font License (https://github.com/liberationfonts/liberation-fonts/tree/2.1.1 https://github.com/liberationfonts/liberation-fonts/blob/master/LICENSE)

## Used ressources
* Format of .wav files from the german Wikipedia (https://de.wikipedia.org/wiki/RIFF_WAVE)
* Article about the Hammond-Organ from the german Wikipedia (https://de.wikipedia.org/wiki/Hammondorgel)
* "Modulation and Delay Line based Digital Audio Effects by Disch Sascha and Zölzer Udo (2002)" helped me implement the rotary speaker (https://www.researchgate.net/publication/2830823_Modulation_And_Delay_Line_Based_Digital_Audio_Effects)
* "Rotary Loudspeaker Effect by Ryan Brennan" helped me implement the rotary speaker (https://ses.library.usyd.edu.au/bitstream/handle/2123/8310/Ryan%20review%20assignment%201.pdf)
* Article about delay lines from McGill university (https://www.music.mcgill.ca/~gary/618/week1/delayline.html)
* Effect Design Part 2 by Jon Datorro (https://ccrma.stanford.edu/~dattorro/EffectDesignPart2.pdf)
* Speed of the rotary speakers (https://www.musiker-board.de/threads/leslie-geschwindigkeiten-in-herz-frequenzen-fuer-die-vb3-orgel-gesucht.511349/)
* Wikipedia article about leslie speakers (https://en.wikipedia.org/wiki/Leslie_speaker)
* Wikipedia article about low-pass-filters (https://en.wikipedia.org/wiki/Low-pass_filter)
* Wikipedia article about high-pass-filter (https://en.wikipedia.org/wiki/High-pass_filter)
* German article about vocoders (https://www.amazona.de/was-ist-ein-vocoder-geschichte-funktionsweise-varianten/)
* Very helpful blog post about PolyBLEP - oscillators for anti-alising by Martin Finke (I wrote my polyblep function based on his) (http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/)
* Article about soft clipping distortion by Eric Tarr (https://www.hackaudio.com/digital-signal-processing/distortion-effects/soft-clipping/)
* Paper about the simulation of a tube amplifier (https://ses.library.usyd.edu.au/bitstream/handle/2123/7624/DESC9115_DAS_Assign02_310106370.pdf;jsessionid=0318BB69B12E37D8810CEDFD50DB5A42?sequence=2)
* Very helpful tutorial on implementing resonant filters by Martin Finke (http://www.martin-finke.de/blog/articles/audio-plugins-013-filter/)
* Implementation of a PolyBLEP oscilator by Martin Finke (my PolyBLEP anti-aliasing is based on this) (https://github.com/martinfinke/PolyBLEP)
* Article about the chorus effect (https://de.wikipedia.org/wiki/Chorus_(Tontechnik))
* https://www.music.mcgill.ca/~gary/307/week1/node24.html
* http://www.dma.ufg.ac.at/app/link/Grundlagen%3AAudio/module/8086?step=all
* https://electricdruid.net/technical-aspects-of-the-hammond-organ/
* https://www.hackaudio.com/digital-signal-processing/distortion-effects/bit-crushing/
* https://de.wikipedia.org/wiki/Bitcrusher
* https://asa.scitation.org/doi/pdf/10.1121/1.5003796
* https://www.dsprelated.com/showarticle/938.php
* https://math.stackexchange.com/questions/178079/integration-of-sawtooth-square-and-triangle-wave-functions
* https://www.kvraudio.com/forum/viewtopic.php?t=445438
* https://stackoverflow.com/questions/3055815/dsp-filter-sweep-effect
* https://www.electronics-tutorials.ws/de/filtern/passiver-tiefpassfilter.html
* https://medium.com/the-seekers-project/coding-a-basic-reverb-algorithm-part-2-an-introduction-to-audio-programming-4db79dd4e325
* Formant frequencies used for the vocoder (https://en.wikipedia.org/wiki/Formant
* Reddit thread about phasers (https://www.reddit.com/r/DSP/comments/2o2pf6/implementing_guitar_phaser_effect/cmjc70p/)
* Fuzz distortion algorithm (http://www.portaudio.com/docs/v19-doxydocs/pa__fuzz_8c.html)
* SpinLock (https://timur.audio/using-locks-in-real-time-audio-processing-safely)

## Other resources that might be useful in the future
* Nice forum post about exponential envelopes (https://www.kvraudio.com/forum/viewtopic.php?t=161416)
* Good envelope shape (https://sfzformat.com/opcodes/ampeg_attack)
