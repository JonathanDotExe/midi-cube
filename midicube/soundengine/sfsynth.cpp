/*
 * sfynth.cpp
 *
 *  Created on: 21 Oct 2020
 *      Author: jojo
 */

#include "sfsynth.h"
#include <boost/filesystem.hpp>

SoundFontSynth::SoundFontSynth() {
	settings = new_fluid_settings();
	fluid_settings_setint(settings, "synth.polyphony", 128);

	synth = new_fluid_synth(settings);

	for (const auto& f : boost::filesystem::directory_iterator("./data/soundfonts")) {
		fluid_synth_sfload(synth, f.path().c_str(), 1);
	}
	fluid_synth_cc(synth, 0, 7, 127); //Set volume
	//TODO use only one synth instance
}

void SoundFontSynth::midi_message(MidiMessage& msg, SampleInfo& info) {
	switch (msg.type) {
	case MessageType::NOTE_ON:
		fluid_synth_noteon(synth, 0, msg.note(), msg.velocity());
		break;
	case MessageType::NOTE_OFF:
		fluid_synth_noteoff(synth, 0, msg.note());
		break;
	case MessageType::CONTROL_CHANGE:
		fluid_synth_cc(synth, 0, msg.control(), msg.value());
		break;
	case MessageType::PITCH_BEND:
		fluid_synth_pitch_bend(synth, 0, msg.get_pitch_bend());
		break;
	default:
		break;
	}
}

void SoundFontSynth::press_note(SampleInfo& info, unsigned int note, double velocity) {
	fluid_synth_noteon(synth, 0, note, velocity * 127);
}

void SoundFontSynth::release_note(SampleInfo& info, unsigned int note) {
	fluid_synth_noteoff(synth, 0, note);
}

void SoundFontSynth::process_sample(double& lsample, double& rsample, SampleInfo& info) {
	float left = 0;
	float right = 0;
	fluid_synth_write_float(synth, 1, &left, 0, 1, &right, 0, 1);

	//Play
	lsample += left * 5;
	rsample += right * 5;
}

SoundFontSynth::~SoundFontSynth() {
	delete_fluid_synth(synth);
	delete_fluid_settings(settings);
};

template<>
std::string get_engine_name<SoundFontSynth>() {
	return "Soundfont Synth";
}

