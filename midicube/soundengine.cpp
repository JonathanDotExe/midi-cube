/*
 * soundengine.cpp
 *
 *  Created on: 28 Jun 2020
 *      Author: jojo
 */

#include "soundengine.h"
#include "synthesis.h"

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

//SoundEngineDevice
SoundEngineDevice::SoundEngineDevice(std::string identifier) {
	this->identifier = identifier;
	engine = new PresetSynth();
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
