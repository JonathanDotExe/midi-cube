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
	MessageType type = msg.get_message_type();
	if (type == MessageType::NOTE_ON) {
		fluid_synth_noteon(synth, 0, msg.get_note(), msg.get_velocity());
	}
	else if (type == MessageType::NOTE_OFF) {
		fluid_synth_noteoff(synth, 0, msg.get_note());
	}
	if (type == MessageType::CONTROL_CHANGE) {
		fluid_synth_cc(synth, 0, msg.get_control(), msg.get_value());
	}
}

void SoundFontSynth::press_note(SampleInfo& info, unsigned int note, double velocity) {
	fluid_synth_noteon(synth, 0, note, velocity * 127);
}

void SoundFontSynth::release_note(SampleInfo& info, unsigned int note) {
	fluid_synth_noteoff(synth, 0, note);
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

template<>
std::string get_engine_name<SoundFontSynth>() {
	return "Soundfont Synth";
}

