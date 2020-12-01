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

void PortInputDevice::set_midi_callback(std::function<void(double, MidiMessage&)> callback) {
	this->input->set_callback(callback);
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

