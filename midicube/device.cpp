/*
 * device.cpp
 *
 *  Created on: 23 Jun 2020
 *      Author: jojo
 */

#include "device.h"
#include "synthesis.h"

//AudioDevice
AudioDevice::AudioDevice() {

}

/*MidiCube* AudioDevice::get_cube() {
	return cube;
}

void AudioDevice::set_cube(MidiCube* cube) {
	if (this->cube == nullptr) {
		this->cube = cube;
	}
	else {
		throw std::runtime_error("Cube already set!");
	}
}*/

AudioDevice::~AudioDevice() {

}



//PortInputDevice
PortInputDevice::PortInputDevice(MidiInput* input, std::string identifier) : AudioDevice() {
	this->input = input;
	this->identifier = identifier;
}

void PortInputDevice::set_midi_callback(void (*recieve) (double, MidiMessage&, void*), void* user_data) {
	this->input->set_callback(recieve, user_data);
}

std::string PortInputDevice::get_identifier() {
	return identifier;
}

PortInputDevice::~PortInputDevice() {
	delete input;
	input = nullptr;
}

//PortOutputDevice
PortOutputDevice::PortOutputDevice(MidiOutput* output, std::string identifier) : AudioDevice() {
	this->output = output;
	this->identifier = identifier;
}

void PortOutputDevice::send(MidiMessage& message) {
	this->output->send(message);
}

std::string PortOutputDevice::get_identifier() {
	return identifier;
}

PortOutputDevice::~PortOutputDevice() {
	delete output;
	output = nullptr;
}

//SoundEngineDevice
SoundEngineDevice::SoundEngineDevice(std::string identifier) {
	this->identifier = identifier;
}

std::string SoundEngineDevice::get_identifier() {
	return identifier;
}

double SoundEngineDevice::process_sample(unsigned int channel, double time) {
	double sample = square_wave(time, freq) * envelope * 0.3;
	return sample;
}

void SoundEngineDevice::send(MidiMessage& message) {
	std::cout << message.to_string() << std::endl;
	if (message.get_message_type() == MessageType::NOTE_ON) {
		freq = note_to_freq(message.get_note());
		envelope = 1;
	}
	else if (message.get_message_type() == MessageType::NOTE_OFF) {
		envelope = 0;
	}
}

SoundEngineDevice::~SoundEngineDevice() {

}

