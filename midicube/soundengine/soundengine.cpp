/*
 * soundengine.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "soundengine.h"

//SoundEngineChannel
SoundEngineChannel::SoundEngineChannel() {
	//Init notes
	for (size_t i = 0; i < note.size(); ++i) {
		note[i].start_time = -1024;
		note[i].release_time = -1024;
	}
}

void SoundEngineChannel::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info) {
	std::array<double, OUTPUT_CHANNELS> ch = {};
	if (engine) {
		for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
			if (!engine->note_finished(info, note[i])) {
				engine->process_note_sample(channels, info, note[i], i);
				note[i].phase_shift += pitch_bend * info.time_step;
			}
		}
		engine->process_sample(channels, info);
	}
	for (size_t i = 0; i < 0; ++i) {
		channels[i] += ch[i];
	}
}

size_t SoundEngineChannel::next_freq_slot() {
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (!note[i].pressed) {
			return i;
		}
	}
	//TODO return longest used slot
	return 0;
}

void SoundEngineChannel::send(MidiMessage &message, SampleInfo& info) {
	//Note on
	if (message.get_message_type() == MessageType::NOTE_ON) {
		size_t slot = next_freq_slot();
		note[slot].freq = note_to_freq(message.get_note());
		note[slot].velocity = message.get_velocity()/127.0;
		note[slot].note = message.get_note();
		note[slot].pressed = true;
		note[slot].start_time = info.time;
		note[slot].release_time = 0;
		note[slot].phase_shift = 0;
	}
	//Note off
	else if (message.get_message_type() == MessageType::NOTE_OFF) {
		double f = note_to_freq(message.get_note());
		for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
			if (note[i].freq == f) {
				note[i].pressed = false;
				note[i].release_time = info.time;
			}
		}
	}
	//Control change
	else if (message.get_message_type() == MessageType::CONTROL_CHANGE) {
		engine->control_change(message.get_control(), message.get_value());
	}
	//Pitch bend
	else if (message.get_message_type() == MessageType::PITCH_BEND) {
		double pitch = (message.get_pitch_bend()/8192.0 - 1.0) * 2;
		pitch_bend = note_to_freq_transpose(pitch) - 1;
	}
}

SoundEngineChannel::~SoundEngineChannel() {
	delete engine;
	engine = nullptr;
}


//SoundEngineDevice
SoundEngineDevice::SoundEngineDevice(std::string identifier) {
	this->identifier = identifier;
}

std::string SoundEngineDevice::get_identifier() {
	return identifier;
}

void SoundEngineDevice::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info) {
	for (size_t i = 0; i < this->channels.size(); ++i) {
		this->channels[i].process_sample(channels, info);
	}
}

std::vector<SoundEngine*> SoundEngineDevice::get_sound_engines() {
	return sound_engines;
}

void SoundEngineDevice::add_sound_engine(SoundEngine* engine) {
	sound_engines.push_back(engine);
}

void SoundEngineDevice::set_engine(unsigned int channel, SoundEngine* engine) {
	this->channels.at(channel).engine = engine;
}

SoundEngine* SoundEngineDevice::get_engine(unsigned int channel) {
	return this->channels.at(channel).engine;
}

void SoundEngineDevice::send(MidiMessage &message) {
	SampleInfo info =  handler->sample_info();
	channels.at(message.get_channel()).send(message, info);
}

SoundEngineDevice::~SoundEngineDevice() {
	for (SoundEngine* engine : sound_engines) {
		delete engine;
	}
	sound_engines.clear();
}
