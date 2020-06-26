/*
 * midicube.cpp
 *
 *  Created on: 17 Jun 2020
 *      Author: jojo
 */

#include "midicube.h"
#include <iostream>

static double process_func(unsigned int channel, double time, void* user_data) {
	return ((MidiCube*) user_data)->process(channel, time);
}

static void midi_callback_func(double deltatime, MidiMessage& msg, void* arg) {
	CallbackUserData* data = (CallbackUserData*) arg;
	data->cube->midi_callback(msg, data->device);
}

MidiCube::MidiCube() {
	audio_handler.set_sample_callback(&process_func, this);
};

void MidiCube::init() {
	create_default_devices();
	audio_handler.init();
};

double MidiCube::process(unsigned int channel, double time) {
	double sig = 0;
	for (std::pair<std::string, AudioDevice*> device : devices) {
		sig += device.second->process_sample(channel, time);
	}
	return sig;
};

void MidiCube::create_default_devices() {
	using namespace std::string_literals;
	//Input-Devices
	MidiInput input_dummy;
	size_t n_ports = input_dummy.available_ports();
	for (size_t i = 0; i < n_ports; ++i) {
		MidiInput* input = new MidiInput();
		std::string name;
		try {
			name = input->port_name(i);
			input->open(i);
		}
		catch (MidiException& exc) {
			delete input;
			input = nullptr;
		}
		if (input != nullptr) {
			PortInputDevice* device = new PortInputDevice(input, "[MIDIIN] "s + name);
			add_device(device);
		}
	}
	//Sound Engine
	SoundEngineDevice* device = new SoundEngineDevice("Sound Engine");
	add_device(device);
	//Bind devices to sound engine, will be removed later
	for (std::pair<std::string, AudioDevice*> dev : devices) {
		if (dev.second->is_midi_input()) {
			DeviceBinding binding{dev.first, device->get_identifier(), -1, -1};
			add_binding(binding);
		}
	}
};

/**
 * Device must be allocated on the heap
 */
void MidiCube::add_device(AudioDevice* device) {
	if (devices.find(device->get_identifier()) == devices.end()) {
		std::cout << "Adding device " << device->get_identifier() << std::endl;
		CallbackUserData* data = new CallbackUserData();
		data->cube = this;
		data->device = device->get_identifier();
		callback_data.push_back(data);
		device->set_midi_callback(&midi_callback_func, data);
		devices[device->get_identifier()] = device;
	}
};

void MidiCube::midi_callback(MidiMessage& message, std::string device) {
	size_t len = bindings[device].size();
	for (size_t i = 0; i < len; ++i) {
		if ((bindings[device][i].input_channel < 0 || bindings[device][i].input_channel == (int) message.get_channel()) && devices.find(device) != devices.end()) {
			MidiMessage msg = message;
			if (bindings[device][i].output_channel >= 0) {
				//TODO set channel
			}
			AudioDevice* dev = devices[bindings[device][i].output];
			dev->send(msg);
		}
	}
}

void MidiCube::add_binding(DeviceBinding binding) {
	bindings[binding.input].push_back(binding);
}

MidiCube::~MidiCube() {
	for (std::pair<std::string, AudioDevice*> device : devices) {
		delete device.second;
	}
	devices.clear();
	//Delete user data
	for (CallbackUserData* data : callback_data) {
		delete data;
	}
	callback_data.clear();
};

