/*
 * sfsynth.h
 *
 *  Created on: 21 Oct 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SFSYNTH_H_
#define MIDICUBE_SOUNDENGINE_SFSYNTH_H_

#include "soundengine.h"
#include <fluidsynth.h>

class SoundFontSynth {

private:
	fluid_settings_t* settings = nullptr;
	fluid_synth_t* synth = nullptr;

public:
	SoundFontSynth();

	void midi_message(MidiMessage& msg, SampleInfo& info);

	void press_note(SampleInfo& info, unsigned int note, double velocity);

	void release_note(SampleInfo& info, unsigned int note);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);

	virtual ~SoundFontSynth();

};



#endif /* MIDICUBE_SOUNDENGINE_SFSYNTH_H_ */
