# midi-cube

A MIDI-Workstation written in C++. Inendend to both work on Single-Chip-Devices with a touch display (Raspberry Pi) and Computers.

## Planned features:
* Serveral integrated virtual instruments like a B3 organ, an E-Piano-synthesizer, a sampler, drumkits, a subtractive/additive/FM-synthesizer, etc.
* MIDI-Routing between devices
* Looper, Arpeggiator, Sequencer/Drum Machine
* A touch responsive GUI
* A web interface or a desktop client that allows uploading new sounds

## Used libraries
* JACK Audio Connection Kit API licensed under LGPL (https://jackaudio.org/api/ https://www.gnu.org/licenses/lgpl-3.0.html)
* RtMidi licensed under the RtMidi license which is pretty similar to the MIT license (https://github.com/thestk/rtmidi https://github.com/thestk/rtmidi/blob/master/LICENSE)
* libsndfile licensed under LGPL (https://github.com/erikd/libsndfile/commits/master https://github.com/erikd/libsndfile/blob/master/COPYING)
* raylib licensed under zlib/libpng license (https://github.com/raysan5/raylib https://github.com/raysan5/raylib/blob/master/LICENSE)
* raygui licensed under zlib/libpng license (https://github.com/raysan5/raygui https://github.com/raysan5/raygui/blob/master/LICENSE)
* JSON library by nlohman licensed under MIT license (https://github.com/nlohmann/json https://github.com/nlohmann/json/blob/develop/LICENSE.MIT)

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
