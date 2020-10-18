/*
 * midicube.cpp
 *
 *  Created on: 17 Jun 2020
 *      Author: jojo
 */

#include "multimidicube.h"
#include <iostream>

static void process_func(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, void* user_data) {
	((MultiMidiCube*) user_data)->process(channels, info);
}

MultiMidiCube::MultiMidiCube() {
	audio_handler.set_sample_callback(&process_func, this);
};

void MultiMidiCube::init() {
	create_default_devices();
	audio_handler.init();
};

void MultiMidiCube::process(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {
	for (std::pair<std::string, AudioDevice*> device : devices) {
		device.second->process_sample(channels, info);
	}
};

void MultiMidiCube::create_default_devices() {
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
	fill_sound_engine_device(device);
	device->get_channel(0).set_engine(0);
	device->get_channel(9).set_engine(2);
	device->get_channel(1).arpeggiator().on = true;
	device->get_channel(1).arpeggiator().preset.pattern = ArpeggiatorPattern::UP;
	device->get_channel(1).arpeggiator().preset.octaves = 3;
	device->get_channel(1).arpeggiator().preset.value = 1;
	device->get_channel(1).arpeggiator().metronome.set_bpm(420);
	device->handler = &audio_handler; //TODO remove, bad practise
	add_device(device);
};

/**
 * Device must be allocated on the heap
 */
void MultiMidiCube::add_device(AudioDevice* device) {
	if (devices.find(device->get_identifier()) == devices.end()) {
		std::cout << "Adding device " << device->get_identifier() << std::endl;
		MultiCallbackUserData* data = new MultiCallbackUserData();
		data->cube = this;
		data->device = device->get_identifier();
		callback_data.push_back(data);
		std::string name = device->get_identifier();
		MultiMidiCube* self = this;
		device->set_midi_callback([&self, &name](double delta, MidiMessage& msg){
			self->midi_callback(msg, name);
		});
		devices[name] = device;
	}
};

void MultiMidiCube::midi_callback(MidiMessage& message, std::string device) {
	size_t len = bindings[device].size();
	MessageType t = message.get_message_type();
	for (size_t i = 0; i < len; ++i) {
		DeviceBinding& b = bindings[device][i];
		if ((b.input_channel < 0 || b.input_channel == (int) message.get_channel()) && devices.find(device) != devices.end()) {
			bool pass = true;
			MidiMessage msg = message;
			switch (t) {
			case MessageType::NOTE_OFF:
			case MessageType::NOTE_ON:
			case MessageType::POLYPHONIC_AFTERTOUCH:
				pass = b.start_note <= message.get_note() && b.end_note >= message.get_note();
				if (b.octave) {
					msg.set_note(msg.get_note() + b.octave * 12);
				}
				break;
			case MessageType::CONTROL_CHANGE:
				pass = b.transfer_cc;
				break;
			case MessageType::PROGRAM_CHANGE:
				pass = b.transfer_prog_change;
				break;
			case MessageType::MONOPHONIC_AFTERTOUCH:
				pass = b.transfer_channel_aftertouch;
				break;
			case MessageType::PITCH_BEND:
				pass = b.transfer_pitch_bend;
				break;
			case MessageType::SYSEX:
				pass = b.transfer_other;
				break;
			case MessageType::INVALID:
				pass = false;
			}
			//Apply binding
			if (pass) {
				if (bindings[device][i].output_channel >= 0) {
					msg.set_channel((unsigned int) bindings[device][i].output_channel);
				}
				AudioDevice* dev = devices[bindings[device][i].output];
				dev->send(msg);
			}
		}
	}
}

void MultiMidiCube::add_binding(DeviceBinding binding) {
	bindings[binding.input].push_back(binding);
}

std::vector<DeviceBinding> MultiMidiCube::get_bindings() {
	std::vector<DeviceBinding> bindings;
	for (auto b : this->bindings) {
		bindings.insert(bindings.end(), b.second.begin(), b.second.end());
	}
	return bindings;
}

std::unordered_map<std::string, AudioDevice*> MultiMidiCube::get_devices() {
	return devices;
}

MultiMidiCube::~MultiMidiCube() {
	//Stop audio thread first to prevent segmentation faults
	audio_handler.close();
	//Delete devices
	for (std::pair<std::string, AudioDevice*> device : devices) {
		delete device.second;
	}
	devices.clear();
	//Delete user data
	for (MultiCallbackUserData* data : callback_data) {
		delete data;
	}
	callback_data.clear();
};
