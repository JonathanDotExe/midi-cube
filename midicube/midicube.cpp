/*
 * midicube.cpp
 *
 *  Created on: 17 Jun 2020
 *      Author: jojo
 */

#include "midicube.h"
#include <iostream>

static void midi_callback(double deltatime, MidiMessage& msg, void* arg) {
	std::cout << "Callback" << std::endl;
	std::cout << msg.to_string() << std::endl;
}

MidiCube::MidiCube() {

};

void MidiCube::init() {

};

void MidiCube::create_default_devices() {
	using namespace std::string_literals;
	//Input-Devices
	MidiInput* input = new MidiInput();
	while (input != nullptr) {
		if (input->available_ports() > 0) {
			std::string name;
			try {
				name = input->port_name(1);
				input->open(0);
			}
			catch (MidiException& exc) {
				delete input;
				input = nullptr;
				std::cerr << "Error opening MIDI Input: " << exc.what() << std::endl;
			}
			if (input != nullptr) {
				PortInputDevice* device = new PortInputDevice(input, "[MIDIIN] "s + name);
				device->set_midi_callback(&midi_callback, nullptr);
				add_device(device);
			}
			input = new MidiInput();
		}
		else {
			delete input;
			input = nullptr;
		}
	}
};

/**
 * Device must be allocated on the heap
 */
void MidiCube::add_device(AudioDevice* device) {
	if (devices.find(device->get_identifier()) == devices.end()) {
		devices[device->get_identifier()] = device;
	}
};

MidiCube::~MidiCube() {
	for (AudioDevice* device : devices) {
		delete device;
	}
	devices.clear();
};

