/*
 * soundengine.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "soundengine.h"

#include <algorithm>

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

void NoteBuffer::release_note(SampleInfo& info, unsigned int note, bool invalidate) {
	double f = note_to_freq(note);
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (this->note[i].freq == f && this->note[i].pressed) {
			this->note[i].pressed = false;
			this->note[i].release_time = info.time;
			if (invalidate) {
				this->note[i].valid = false;
			}
		}
	}
}

//Arpeggiator
Arpeggiator::Arpeggiator() {

}

void Arpeggiator::apply(SampleInfo& info, NoteBuffer& note) {
	//Reset if no keys are pressed
	if (!restart) {
		bool released = true;
		for (size_t i = 0; i < this->note.note.size() && released; ++i) {
			released = !this->note.note[i].pressed;
		}
		restart = released;
	}
	if (restart) {
		//Keyboard sync
		metronome.init(info.sample_time);
	}
	//Pattern
	if (metronome.is_beat(info.sample_time, info.sample_rate, preset.value)) {
		unsigned int next_note = 128;
		long int next_index = -1;

		unsigned int lowest_note = 128;
		long int lowest_index = -1;
		int highest_note = -1;
		long int highest_index = -1;

		switch (preset.pattern) {
		case ArpeggiatorPattern::UP:
			for (size_t i = 0; i < this->note.note.size(); ++i) {
				if (this->note.note[i].pressed) {
					if (this->note.note[i].note < lowest_note) {
						lowest_note = this->note.note[i].note;
						lowest_index = i;
					}
					//Find next highest note
					for (unsigned int octave = 0; octave < preset.octaves; ++octave) {
						unsigned int n = this->note.note[i].note + octave * 12;
						if (n < next_note && (n > curr_note || (n == curr_note && note_index > i))) {
							next_note = n;
							next_index = i;
							break;
						}
					}
				}
			}
			//Restart from lowest
			if (restart || next_index < 0) {
				next_note = lowest_note;
				next_index = lowest_index;
			}
			break;
		case ArpeggiatorPattern::DOWN:
			for (size_t i = 0; i < this->note.note.size(); ++i) {
				if (this->note.note[i].pressed) {
					if ((int) (this->note.note[i].note + (preset.octaves - 1) * 12) > highest_note) {
						highest_note = this->note.note[i].note  + (preset.octaves - 1) * 12;
						highest_index = i;
					}
					//Find next lowest note
					for (unsigned int o = 0; o < preset.octaves; ++o) {
						unsigned int octave = preset.octaves - o - 1;
						unsigned int n = this->note.note[i].note + octave * 12;
						if (n > next_note && (n > curr_note || (n == curr_note && note_index > i))) {
							next_note = n;
							next_index = i;
							break;
						}
					}
				}
			}
			//Restart from highest
			if (restart || next_index < 0) {
				next_note = highest_note;
				next_index = highest_index;
			}
			break;
		case ArpeggiatorPattern::UP_DOWN:
			//TODO
			break;
		case ArpeggiatorPattern::RANDOM:
			//TODO
			break;
		case ArpeggiatorPattern::UP_CUSTOM:
			break;
		case ArpeggiatorPattern::DOWN_CUSTOM:
			break;
		}
		//Press note
		note.release_note(info, curr_note);
		if (next_index >= 0) {
			curr_note = next_note;
			this->note_index = next_index;
			note.press_note(info, curr_note, this->note.note[note_index].velocity);
			restart = false;
		}
	}
}

void Arpeggiator::press_note(SampleInfo& info, unsigned int note, double velocity) {
	this->note.press_note(info, note, velocity);
}

void Arpeggiator::release_note(SampleInfo& info, unsigned int note) {
	this->note.release_note(info, note, true);
}

//SoundEngineChannel
SoundEngineChannel::SoundEngineChannel() {
	engine = nullptr;
	data = nullptr;
}

void SoundEngineChannel::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, Metronome& metronome) {
	std::array<double, OUTPUT_CHANNELS> ch = {};
	engine_mutex.lock();
	if (engine && data) {
		if (arp.on) {
			arp.apply(info, note);
		}
		if (active) {
			EngineStatus status = {0};
			//Notes
			for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
				if (note.note[i].valid) {
					if (engine->note_finished(info, note.note[i], environment, *data)) {
						note.note[i].valid = false;
						engine->note_not_pressed(info, note.note[i], *data, i);
					}
					else {
						++status.pressed_notes;
						engine->process_note_sample(ch, info, note.note[i], environment, *data, i);
						note.note[i].phase_shift += (environment.pitch_bend - 1) * info.time_step;
					}
				}
				else {
					engine->note_not_pressed(info, note.note[i], *data, i);
				}
			}
			//Static sample
			engine->process_sample(ch, info, environment, status, *data);
		}
		//Looper
		looper.apply(ch, metronome, info);
		//Playback
		for (size_t i = 0; i < channels.size(); ++i) {
			channels[i] += (ch[i] * volume);
		}
	}
	engine_mutex.unlock();
}

void SoundEngineChannel::send(MidiMessage &message, SampleInfo& info) {
	engine_mutex.lock();
	//Note on
	if (message.get_message_type() == MessageType::NOTE_ON) {
		if (!arp.on) {
			note.press_note(info, message.get_note(), message.get_velocity()/127.0);
		}
		arp.press_note(info, message.get_note(), message.get_velocity()/127.0);
	}
	//Note off
	else if (message.get_message_type() == MessageType::NOTE_OFF) {
		if (!arp.on) {
			note.release_note(info, message.get_note());
		}
		arp.release_note(info, message.get_note());
	}
	//Control change
	else if (message.get_message_type() == MessageType::CONTROL_CHANGE) {
		if (engine && data) {
			engine->control_change(message.get_control(), message.get_value(), *data);
		}
		//Sustain
		if (message.get_control() == sustain_control) {
			bool new_sustain = message.get_value() != 0;
			if (new_sustain != environment.sustain) {
				if (new_sustain) {
					environment.sustain_time = info.time;
				}
				else {
					environment.sustain_release_time = info.time;
				}
				environment.sustain = new_sustain;
			}
		}
	}
	//Pitch bend
	else if (message.get_message_type() == MessageType::PITCH_BEND) {
		double pitch = (message.get_pitch_bend()/8192.0 - 1.0) * 2;
		environment.pitch_bend = note_to_freq_transpose(pitch);
	}
	engine_mutex.unlock();
}

/**
 * May only be called from GUI thread after GUI has started
 */
void SoundEngineChannel::set_engine(SoundEngine* engine) {
	SoundEngineData* data = nullptr;
	if (engine) {
		data = engine->create_data();
	}

	engine_mutex.lock();
	delete this->data;
	this->engine = engine;
	this->data = data;
	engine_mutex.unlock();
}

ssize_t SoundEngineChannel::get_engine_index(std::vector<SoundEngine*>& engines) {
	engine_mutex.lock();
	ssize_t index = find(engines.begin(), engines.end(), engine) - engines.begin();
	engine_mutex.unlock();
	if (index == engines.end() - engines.begin()) {
		index = -1;
	}
	return index;
}

SoundEngineData* SoundEngineChannel::get_data() {
	return data;
}

Arpeggiator& SoundEngineChannel::arpeggiator() {
	return arp;
}

Looper& SoundEngineChannel::get_looper() {
	return looper;
}

SoundEngineChannel::~SoundEngineChannel() {
	set_engine(nullptr);
}


//SoundEngineDevice
SoundEngineDevice::SoundEngineDevice(std::string identifier) {
	this->identifier = identifier;
	metronome.init(0);
}

std::string SoundEngineDevice::get_identifier() {
	return identifier;
}

void SoundEngineDevice::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info) {
	//Channels
	for (size_t i = 0; i < this->channels.size(); ++i) {
		this->channels[i].process_sample(channels, info, metronome);
	}
	//Metronome
	if (play_metronome) {
		double start =(double) metronome.last_beat(info.sample_time, info.sample_rate, 1)/info.sample_rate;
		double vol = metronome_env.amplitude(info.time, true, start, 0);
		if (vol) {
			double sample = sine_wave(info.time, 3520) * vol;
			for (size_t i = 0; i < channels.size(); ++i) {
				channels[i] += sample;
			}
		}
	}
}

std::vector<SoundEngine*> SoundEngineDevice::get_sound_engines() {
	return sound_engines;
}

void SoundEngineDevice::add_sound_engine(SoundEngine* engine) {
	sound_engines.push_back(engine);
}

SoundEngineChannel& SoundEngineDevice::get_channel(unsigned int channel) {
	return channels.at(channel);
}

std::string SoundEngineChannel::get_engine_name() {
	engine_mutex.lock();
	std::string name = engine == nullptr ? "None" : engine->get_name();
	engine_mutex.unlock();
	return name;
}

void SoundEngineDevice::send(MidiMessage &message) {
	SampleInfo info =  handler->sample_info();
	channels.at(message.get_channel()).send(message, info);
}

void SoundEngineDevice::solo (unsigned int channel) {
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i].active = channel == i;
	}
}


SoundEngineDevice::~SoundEngineDevice() {
	//Clear channels
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i].set_engine(nullptr);
	}
	//Delete engines
	for (SoundEngine* engine : sound_engines) {
		delete engine;
	}
	sound_engines.clear();
}
