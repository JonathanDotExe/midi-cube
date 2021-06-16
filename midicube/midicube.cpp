/*
 * midicube.cpp
 *
 *  Created on: Oct 3, 2020
 *      Author: jojo
 */

#include "midicube.h"
#include "soundengine/engines.h"
#include <iostream>

static void process_func(double& lsample, double& rsample, SampleInfo& info, void* user_data) {
	((MidiCube*) user_data)->process(lsample, rsample, info);
}

MidiCube::MidiCube() : prog_mgr("./data/programs", action_handler) {
	audio_handler.set_sample_callback(&process_func, this);
}

void MidiCube::init(int out_device, int in_device) {
	global_sample_store.load_sounds("./data/samples");
	//Sound Engines
	fill_sound_engine_device(&engine);
	//Default setting
	Arpeggiator& arp = engine.channels[1].arp;
	arp.on = true;
	arp.preset.octaves = 3;
	arp.preset.pattern = ArpeggiatorPattern::ARP_UP;
	arp.preset.value = 1;
	arp.metronome.set_bpm(440);

	//Default engines
	engine.channels[0].scenes[0].active = true;
	//engine.channels[0].vocoder_preset.on = true;

	engine.channels[0].set_engine_index(2);

	engine.channels[9].scenes[0].active = true;
	engine.channels[9].set_engine_index(3);

	//engine.channels[10].bitcrusher_preset.on = true;
	//engine.channels[10].bitcrusher_preset.bits = 8;

	for (size_t i = 0; i < engine.channels.size(); ++i) {
		engine.channels[i].scenes[0].source.channel = i;
		engine.channels[i].scenes[0].source.input = 1;
	}
	//Load programs
	prog_mgr.init_user(this);
	prog_mgr.load_all();
	prog_mgr.apply_program_direct(0, 0);

	//MIDI Inputs
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
			inputs.push_back({input, name});
		}
	}
	srand(time(NULL));
	//Init audio
	audio_handler.init(out_device, in_device);
}

void MidiCube::process(double& lsample, double& rsample, SampleInfo& info) {
	//Lock actions
	action_handler.execute_realtime_actions();
	//Messages
	size_t i = 0;
	for (MidiCubeInput in : inputs) {
		MidiMessage msg;
		while (in.in->read(&msg)) {
			process_midi(msg, i);
		}
		++i;
	}
	//Process
	engine.process_sample(lsample, rsample, info);
}

std::vector<MidiCubeInput> MidiCube::get_inputs() {
	return inputs;
}

inline void MidiCube::process_midi(MidiMessage& message, size_t input) {
	SampleInfo info = audio_handler.sample_info();
	if (engine.send(message, input, info)) {
		updated = true;
	}
}

MidiCube::~MidiCube() {
	//Load programs
	prog_mgr.save_all();
	audio_handler.close();
	for (MidiCubeInput in : inputs) {
		delete in.in;
	}
	inputs.clear();
}

void MidiCube::save_program(Program *prog) {
	engine.save_program(prog);
	std::cout << "Saved program: " << prog->name << std::endl;
}

void MidiCube::apply_program(Program *prog) {
	engine.apply_program(prog);
	std::cout << "Selected program: " << prog->name << std::endl;
}
