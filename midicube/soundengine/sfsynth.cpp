/*
 * sfynth.cpp
 *
 *  Created on: 21 Oct 2020
 *      Author: jojo
 */

#include "sfsynth.h"

SoundFontSynth::SoundFontSynth() {
	settings = new_fluid_settings();
	fluid_settings_setint(settings, "synth.polyphony", 128);

	synth = new_fluid_synth(settings);
}

void SoundFontSynth::midi_message(MidiMessage& msg, SampleInfo& info) {

}

void SoundFontSynth::press_note(SampleInfo& info, unsigned int note, double velocity) {

}

void SoundFontSynth::release_note(SampleInfo& info, unsigned int note) {

}

void SoundFontSynth::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {
	float left = 0;
	float right = 0;
	fluid_synth_write_float(synth, 1, &left, 0, 1, &right, 0, 1);

	//Play
	for (size_t i = 0; i < channels.size() ; ++i) {
		if (i % 2 == 0) {
			channels[i] += left;
		}
		else {
			channels[i] += right;
		}
	}
}

SoundFontSynth::~SoundFontSynth() {
	delete_fluid_synth(synth);
	delete_fluid_settings(settings);
};


