/*
 * midicube.cpp
 *
 *  Created on: Oct 3, 2020
 *      Author: jojo
 */

#include "midicube.h"
#include "soundengine/organ.h"
#include "soundengine/drums.h"
#include "soundengine/presetsynth.h"
#include "soundengine/sampler.h"
#include "soundengine/vocoder.h"
#include "soundengine/synthesizer.h"
#include "soundengine/sfsynth.h"

static void process_func(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, void* user_data) {
	((MidiCube*) user_data)->process(channels, info);
}

MidiCube::MidiCube() {
	audio_handler.set_sample_callback(&process_func, this);
	engine.handler = &audio_handler;
}

void MidiCube::init() {
	//MIDI Inputs
	//Input-Devices
	MidiInput input_dummy;
	size_t n_ports = input_dummy.available_ports();
	for (size_t i = 0; i < n_ports; ++i) {
		MidiInput* input = new MidiInput();
		std::string name;
		try {
			name = input->port_name(i);
			input->open(i);
		}
		catch (MidiException& exc) {
			delete input;
			input = nullptr;
		}
		if (input != nullptr) {
			inputs.push_back({input, name});
			size_t index = inputs.size() - 1;
			input->set_callback([index, this](double delta, MidiMessage& msg) {
				midi_callback(msg, index);
			});
		}
	}
	//Sound Engines
	engine.add_sound_engine(new TemplateSoundEngineBank<PresetSynth>());
	engine.add_sound_engine(new TemplateSoundEngineBank<B3Organ>());
	engine.add_sound_engine(new TemplateSoundEngineBank<SampleDrums>());
	engine.add_sound_engine(new TemplateSoundEngineBank<Sampler>());
	engine.add_sound_engine(new TemplateSoundEngineBank<Vocoder>());
	engine.add_sound_engine(new TemplateSoundEngineBank<Synthesizer>());
	engine.add_sound_engine(new TemplateSoundEngineBank<SoundFontSynth>());
	//Init audio
	audio_handler.init();
}

void MidiCube::process(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {
	engine.process_sample(channels, info);
}

std::vector<MidiCubeInput> MidiCube::get_inputs() {
	return inputs;
}

void MidiCube::midi_callback(MidiMessage& message, size_t input) {
	MessageType t = message.get_message_type();
	for (size_t i = 0; i < channels.size(); ++i) {
		ChannelSource& s = channels[i];
		if (s.input == static_cast<ssize_t>(input) && s.channel == message.get_channel()) {
			bool pass = true;
			MidiMessage msg = message;
			switch (t) {
			case MessageType::NOTE_OFF:
			case MessageType::NOTE_ON:
			case MessageType::POLYPHONIC_AFTERTOUCH:
				pass = s.start_note <= message.get_note() && s.end_note >= message.get_note();
				if (s.octave) {
					msg.set_note(msg.get_note() + s.octave * 12);
				}
				break;
			case MessageType::CONTROL_CHANGE:
				pass = s.transfer_cc;
				break;
			case MessageType::PROGRAM_CHANGE:
				pass = s.transfer_prog_change;
				break;
			case MessageType::MONOPHONIC_AFTERTOUCH:
				pass = s.transfer_channel_aftertouch;
				break;
			case MessageType::PITCH_BEND:
				pass = s.transfer_pitch_bend;
				break;
			case MessageType::SYSEX:
				pass = s.transfer_other;
				break;
			case MessageType::INVALID:
				pass = false;
			}
			//Apply binding
			if (pass) {
				msg.set_channel(i);
				engine.send(msg);
			}
		}
	}
}
MidiCube::~MidiCube() {
	audio_handler.close();
	for (MidiCubeInput in : inputs) {
		delete in.in;
	}
	inputs.clear();
}
