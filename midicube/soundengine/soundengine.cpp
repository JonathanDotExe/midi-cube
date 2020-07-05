/*
 * soundengine.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "soundengine.h"


//SoundEngineDevice
SoundEngineDevice::SoundEngineDevice(SoundEngine* engine, std::string identifier) {
	this->identifier = identifier;
	this->engine = engine;
	//Init notes
	for (size_t i = 0; i < note.size(); ++i) {
		note[i].start_time = -1024;
		note[i].release_time = -1024;
	}
}

std::string SoundEngineDevice::get_identifier() {
	return identifier;
}

void SoundEngineDevice::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info) {
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (!engine->note_finished(info, note[i])) {
			engine->process_note_sample(channels, info, note[i]);
			note[i].phase_shift += pitch_bend * info.time_step;
		}
	}
	engine->process_sample(channels, info);
}

size_t SoundEngineDevice::next_freq_slot() {
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (!note[i].pressed) {
			return i;
		}
	}
	//TODO return longest used slot
	return 0;
}

void SoundEngineDevice::send(MidiMessage &message) {
	//Note on
	if (message.get_message_type() == MessageType::NOTE_ON) {
		size_t slot = next_freq_slot();
		note[slot].freq = note_to_freq(message.get_note());
		note[slot].velocity = message.get_velocity()/127.0;
		note[slot].note = message.get_note();
		note[slot].pressed = true;
		note[slot].start_time = handler->sample_info().time;
		note[slot].release_time = 0;
		note[slot].phase_shift = 0;
	}
	//Note off
	else if (message.get_message_type() == MessageType::NOTE_OFF) {
		double f = note_to_freq(message.get_note());
		for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
			if (note[i].freq == f) {
				note[i].pressed = false;
				note[i].release_time = handler->sample_info().time;
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

SoundEngineDevice::~SoundEngineDevice() {
	delete engine;
	engine = nullptr;
}
