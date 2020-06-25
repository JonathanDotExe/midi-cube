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
	//TODO Initialize
};

void create_default_devices() {
	//Input-Devices
	MidiInput* input = new MidiInput();
	while (input != nullptr) {
		if (input->available_ports() > 0) {
			try {
				std::string name = input->port_name(1);
				input->open(0);
				PortInputDevice* device = new PortInputDevice(input, name);
				device->set_midi_callback(&midi_callback, nullptr);
			}
			catch (MidiException& exc) {
				delete input;
				input = nullptr;
				std::cerr << "Error opening MIDI Input: " << exc.what() << std::endl;
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
	devices[device->get_identifier()] = device;
};

MidiCube::~MidiCube() {
	//TODO Clean up ressources
};

