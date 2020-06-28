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

double PresetSynth::process_sample(unsigned int channel, double time, double freq) {
	double sample = 0.0;
	sample += saw_wave(time, freq);
	sample += saw_wave(time, freq * detune);
	sample += saw_wave(time, freq * ndetune);

	return sample * 0.1;
}

std::string PresetSynth::get_name() {
	return "Preset Synth";
}

//B3Organ
B3Organ::B3Organ() {
	drawbar_harmonics = {0.5, 0.5 * 3, 1, 2, 3, 4, 5, 6, 8};
}

double B3Organ::process_sample(unsigned int channel, double time, double freq) {
	double sample = 0;

	//Organ sound
	for (size_t i = 0;  i < data.drawbars.size(); ++i) {
		double f = freq * drawbar_harmonics[i];
		while (f > 5593) {
			f /= 2.0;
		}
		sample += data.drawbars[i]/8.0 * sine_wave(time, f);
	}
	sample /= 9.0;

	std::cout << sample << std::endl;

	return sample;
}

std::string B3Organ::get_name() {
	return "B3 Organ";
}

//SoundEngineDevice
SoundEngineDevice::SoundEngineDevice(std::string identifier) {
	this->identifier = identifier;
	engine = new B3Organ();
}

std::string SoundEngineDevice::get_identifier() {
	return identifier;
}

double SoundEngineDevice::process_sample(unsigned int channel, double time) {
	double sample = 0.0;
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (amplitude[i]) {
			sample += engine->process_sample(channel, time, freq[i]) * amplitude[i];
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
