/*
 * soundengine.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "soundengine.h"


//NoteBuffer
NoteBuffer::NoteBuffer () {
	//Init notes
	for (size_t i = 0; i < note.size(); ++i) {
		note[i].start_time = -1024;
		note[i].release_time = -1024;
	}
}

size_t NoteBuffer::next_freq_slot(SampleInfo& info) {
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (!note[i].valid) {
			return i;
		}
	}
	//TODO return longest used slot
	return 0;
}

void NoteBuffer::press_note(SampleInfo& info, unsigned int note, double velocity) {
	size_t slot = next_freq_slot(info);
	this->note[slot].freq = note_to_freq(note);
	this->note[slot].velocity = velocity;
	this->note[slot].note = note;
	this->note[slot].pressed = true;
	this->note[slot].start_time = info.time;
	this->note[slot].release_time = 0;
	this->note[slot].phase_shift = 0;
	this->note[slot].valid = true;
}

void NoteBuffer::release_note(SampleInfo& info, unsigned int note) {
	double f = note_to_freq(note);
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (this->note[i].freq == f && this->note[i].pressed) {
			this->note[i].pressed = false;
			this->note[i].release_time = info.time;
		}
	}
}


//SoundEngineChannel
SoundEngineChannel::SoundEngineChannel() {

}

void SoundEngineChannel::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info) {
	std::array<double, OUTPUT_CHANNELS> ch = {};
	if (engine) {
		for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
			if (note.note[i].valid) {
				if (engine->note_finished(info, note.note[i], environment)) {
					note.note[i].valid = false;
					engine->note_not_pressed(info, note.note[i], i);
				}
				else {
					engine->process_note_sample(ch, info, note.note[i], environment, i);
					note.note[i].phase_shift += pitch_bend * info.time_step;
				}
			}
			else {
				engine->note_not_pressed(info, note.note[i], i);
			}
		}
		engine->process_sample(ch, info);
		for (size_t i = 0; i < channels.size(); ++i) {
			channels[i] += (ch[i] * volume);
		}
	}
}

void SoundEngineChannel::send(MidiMessage &message, SampleInfo& info) {
	unsigned int channel = message.get_channel();
	std::cout << channel << std::endl;
	//Note on
	if (message.get_message_type() == MessageType::NOTE_ON) {
		note.press_note(info, message.get_note(), message.get_velocity()/127.0);
	}
	//Note off
	else if (message.get_message_type() == MessageType::NOTE_OFF) {
		note.release_note(info, message.get_note());
	}
	//Control change
	else if (message.get_message_type() == MessageType::CONTROL_CHANGE) {
		engine->control_change(message.get_control(), message.get_value());
		//Sustain
		if (message.get_control() == sustain_control) {
			bool new_sustain = message.get_value() != 0;
			if (new_sustain != environment.sustain) {
				if (new_sustain) {
					environment.sustain_time = info.time;
				}
				else {
					environment.sustain_release_time = info.time;
					std::cout << "Release sustain " << environment.sustain_release_time << std::endl;
				}
				environment.sustain = new_sustain;
			}
		}
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

double& SoundEngineDevice::volume(unsigned int channel) {
	return channels.at(channel).volume;
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
