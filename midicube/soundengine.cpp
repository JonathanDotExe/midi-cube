/*
 * soundengine.cpp
 *
 *  Created on: 28 Jun 2020
 *      Author: jojo
 */

#include "soundengine.h"
#include "synthesis.h"
#include <iostream>

//PresetSynth
PresetSynth::PresetSynth() {
	detune = note_to_freq_transpose(0.1);
	ndetune = note_to_freq_transpose(-0.1);
}

double PresetSynth::process_sample(unsigned int channel, SampleInfo& info, double freq) {
	double sample = 0.0;
	sample += saw_wave(info.time, freq);
	sample += saw_wave(info.time, freq * detune);
	sample += saw_wave(info.time, freq * ndetune);

	return sample * 0.1;
}

std::string PresetSynth::get_name() {
	return "Preset Synth";
}

//B3Organ
#define SPEAKER_RADIUS 0.25
#define HORN_RADIUS 0.15
#define BASS_RADIUS 0.15
#define SOUND_SPEED 343.2

B3Organ::B3Organ() {
	drawbar_harmonics = {0.5, 0.5 * 3, 1, 2, 3, 4, 5, 6, 8};
}

static inline double sound_delay(double rotation, double radius) {
	double dst = rotation >= 0 ? (SPEAKER_RADIUS - radius * rotation) : (SPEAKER_RADIUS + radius * rotation + radius * 2);
	return dst/SOUND_SPEED;
}

double B3Organ::process_sample(unsigned int channel, SampleInfo& info, double freq) {
	double horn_sample = 0;
	double bass_sample = 0;

	//Organ sound
	for (size_t i = 0;  i < data.drawbars.size(); ++i) {
		double f = freq * drawbar_harmonics[i];
		while (f > 5593) {
			f /= 2.0;
		}
		if (f > ROTARY_CUTOFF) {
			horn_sample += data.drawbars[i]/8.0 * sine_wave(info.time, f);
		}
		else {
			bass_sample += data.drawbars[i]/8.0 * sine_wave(info.time, f);
		}
	}
	horn_sample /= data.drawbars.size();
	bass_sample /= data.drawbars.size();
	double sample = 0;

	//Rotary speaker
	if (data.rotary) {
		double mul = channel % 2 == 0 ? 1 : -1;

		double horn_speed = data.rotary_fast ? ROTARY_HORN_FAST_FREQUENCY : ROTARY_HORN_SLOW_FREQUENCY;
		double bass_speed = data.rotary_fast ? ROTARY_BASS_FAST_FREQUENCY : ROTARY_BASS_SLOW_FREQUENCY;

		//Horn
		DelaySample samp_horn{info.time + sound_delay(sine_wave(info.time, horn_speed) * mul, HORN_RADIUS), horn_sample};
		//Bass
		DelaySample samp_bass{info.time + sound_delay(sine_wave(info.time, bass_speed) * mul, BASS_RADIUS), bass_sample};

		//Horn
		//DelaySample samp_horn{time + (1 + sine_wave(time, horn_speed)) * HORN_RADIUS/SOUND_SPEED, horn_sample};
		//Bass
		//DelaySample samp_bass{time + (1 + sine_wave(time, bass_speed)) * BASS_RADIUS/SOUND_SPEED, bass_sample};

		//Process
		if (channel % 2 == 0) {
			if (horn_sample) {
				left_horn_del.add_sample(samp_horn);
			}
			if (bass_sample) {
				left_bass_del.add_sample(samp_bass);
			}
			sample += left_horn_del.process(info.time);
			sample += left_bass_del.process(info.time);
		}
		else {
			if (horn_sample) {
				right_horn_del.add_sample(samp_horn);
			}
			if (bass_sample) {
				right_bass_del.add_sample(samp_bass);
			}
			sample += right_horn_del.process(info.time);
			sample += right_bass_del.process(info.time);
		}
	}
	else {
		sample += horn_sample + bass_sample;
	}

	return sample * 0.1;
}

std::string B3Organ::get_name() {
	return "B3 Organ";
}

//SoundEngineDevice
SoundEngineDevice::SoundEngineDevice(std::string identifier) {
	this->identifier = identifier;
	engine = new PresetSynth();
}

std::string SoundEngineDevice::get_identifier() {
	return identifier;
}

double SoundEngineDevice::process_sample(unsigned int channel, SampleInfo& info) {
	double sample = 0.0;
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (amplitude[i]) {
			sample += engine->process_sample(channel, info, freq[i]) * amplitude[i];
		}
	}
	return sample;
}

size_t SoundEngineDevice::next_freq_slot() {
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (!amplitude[i]) {
			return i;
		}
	}
	//TODO return longest used slot
	return 0;
}

void SoundEngineDevice::send(MidiMessage& message) {
	std::cout << message.to_string() << std::endl;
	if (message.get_message_type() == MessageType::NOTE_ON) {
		size_t slot = next_freq_slot();
		std::cout << slot << std::endl;
		freq[slot] = note_to_freq(message.get_note());
		amplitude[slot] = 1;
	}
	else if (message.get_message_type() == MessageType::NOTE_OFF) {
		double f  = note_to_freq(message.get_note());
		for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
			if (freq[i] == f) {
				freq[i] = 0;
				amplitude[i] = 0;
			}
		}
	}
}

SoundEngineDevice::~SoundEngineDevice() {
	delete engine;
	engine = nullptr;
}
