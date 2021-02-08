# midi-cube

A MIDI-Workstation written in C++. Intendend to both work on Single-Chip-Devices with a touch display (Raspberry Pi) and Computers.

## Planned features:
* Serveral integrated virtual instruments like a B3 organ, an E-Piano-synthesizer, a sampler, drumkits, a subtractive/additive/FM-synthesizer, etc.
* MIDI-Routing between devices
* Looper, Arpeggiator, Sequencer/Drum Machine
* A touch responsive GUI
* A web interface or a desktop client that allows uploading new sounds

## Used libraries
* RtAudio licensed under the RtAudio License which is pretty similar to the MIT License (https://github.com/thestk/rtaudio https://github.com/thestk/rtaudio/blob/master/LICENSE)
* RtMidi licensed under the RtMidi license which is pretty similar to the MIT license (https://github.com/thestk/rtmidi https://github.com/thestk/rtmidi/blob/master/LICENSE)
* libsndfile licensed under LGPL (https://github.com/erikd/libsndfile/commits/master https://github.com/erikd/libsndfile/blob/master/COPYING)
* FluidSynth API for playing soundfonts licensed under LGPL-2.1 (https://github.com/FluidSynth/fluidsynth https://github.com/FluidSynth/fluidsynth/blob/master/LICENSE)
* Boost licensed under the Boost Software License (https://github.com/boostorg/boost https://github.com/boostorg/boost/blob/master/LICENSE_1_0.txt)
* SFML licensed under the zlib/png license (https://github.com/SFML/SFML https://github.com/SFML/SFML/blob/master/license.md)

## Used Assets
* Liberation font licensed onder the Open Font License (https://github.com/liberationfonts/liberation-fonts/tree/2.1.1 https://github.com/liberationfonts/liberation-fonts/blob/master/LICENSE)

## Used ressources
* Format of .wav files from the german Wikipedia (https://de.wikipedia.org/wiki/RIFF_WAVE)
* Article about the Hammond-Organ from the german Wikiedia (https://de.wikipedia.org/wiki/Hammondorgel)
* Modulation and Delay Line based Digital Audio Effects by Disch Sascha and Zölzer Udo (2002); helped me implement the rotary speaker (https://www.researchgate.net/publication/2830823_Modulation_And_Delay_Line_Based_Digital_Audio_Effects)
* Rotary Loudspeaker Effect by Ryan Brennan; helped me implementent the rotary speaker (https://ses.library.usyd.edu.au/bitstream/handle/2123/8310/Ryan%20review%20assignment%201.pdf)
* Article about delay lines from McGill university (https://www.music.mcgill.ca/~gary/618/week1/delayline.html)
* Effect Design Part 2 by Jon Datorro (https://ccrma.stanford.edu/~dattorro/EffectDesignPart2.pdf)
* Speed of the rotary speakers (https://www.musiker-board.de/threads/leslie-geschwindigkeiten-in-herz-frequenzen-fuer-die-vb3-orgel-gesucht.511349/)
* Wikipedia article about leslie speakers (https://en.wikipedia.org/wiki/Leslie_speaker)
* Wikipedia article about low-pass-filters (https://en.wikipedia.org/wiki/Low-pass_filter)
* Wikipedia article about high-pass-filter (https://en.wikipedia.org/wiki/High-pass_filter)
* German articale about vocoders (https://www.amazona.de/was-ist-ein-vocoder-geschichte-funktionsweise-varianten/)
* Very helpful blog post about PolyBLEP - oscilators for anti-alising by Martin Finke (I wrote my polyblep function based on his) (http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/)
* Artivle about soft clipping distortion by Eric Tarr (https://www.hackaudio.com/digital-signal-processing/distortion-effects/soft-clipping/)
* Paper about simulation a tube amplifier (https://ses.library.usyd.edu.au/bitstream/handle/2123/7624/DESC9115_DAS_Assign02_310106370.pdf;jsessionid=0318BB69B12E37D8810CEDFD50DB5A42?sequence=2)
* Very helpful tutorial on implementing resonant filters by Martin Finke (http://www.martin-finke.de/blog/articles/audio-plugins-013-filter/)
* Implementation of a PolyBLEP oscilator by Martin Finke (my PolyBLEP anti-aliasing is based on this) (https://github.com/martinfinke/PolyBLEP) 
* http://www.dma.ufg.ac.at/app/link/Grundlagen%3AAudio/module/8086?step=all
* https://electricdruid.net/technical-aspects-of-the-hammond-organ/
* https://www.hackaudio.com/digital-signal-processing/distortion-effects/bit-crushing/
* https://de.wikipedia.org/wiki/Bitcrusher
* https://asa.scitation.org/doi/pdf/10.1121/1.5003796
* https://www.dsprelated.com/showarticle/938.php
* https://math.stackexchange.com/questions/178079/integration-of-sawtooth-square-and-triangle-wave-functions
