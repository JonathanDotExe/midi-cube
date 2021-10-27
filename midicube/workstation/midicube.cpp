/*
 * midicube.cpp
 *
 *  Created on: Oct 3, 2020
 *      Author: jojo
 */

#include "midicube.h"
#include <iostream>


#include "../plugins/soundengine/organ.h"
#include "../plugins/soundengine/drums.h"
#include "../plugins/soundengine/sampler.h"
#include "../plugins/soundengine/asynth.h"

#include "../plugins/effect/amplifier_simulation.h"
#include "../plugins/effect/bitcrusher.h"
#include "../plugins/effect/chorus.h"
#include "../plugins/effect/compressor.h"
#include "../plugins/effect/delay.h"
#include "../plugins/effect/equalizer.h"
#include "../plugins/effect/flanger.h"
#include "../plugins/effect/looper.h"
#include "../plugins/effect/phaser.h"
#include "../plugins/effect/reverb.h"
#include "../plugins/effect/rotary_speaker.h"
#include "../plugins/effect/tremolo.h"
#include "../plugins/effect/vocoder.h"
#include "../plugins/effect/wahwah.h"

#include "../plugins/sequencer/arpeggiator.h"


static void process_func(double& lsample, double& rsample, double* inputs, const size_t input_count, SampleInfo& info, void* user_data) {
	((MidiCube*) user_data)->process(lsample, rsample, inputs, input_count, info);
}

MidiCube::MidiCube() : prog_mgr("./data/programs") {
	audio_handler.set_sample_callback(&process_func, this);
}

void MidiCube::init(int out_device, int in_device) {
	engine.init(this);
	//Sound Engines
	plugin_mgr.add_plugin(new AdvancedSynthPlugin());
	plugin_mgr.add_plugin(new OrganPlugin());
	plugin_mgr.add_plugin(new SamplerPlugin());
	plugin_mgr.add_plugin(new SampleDrumsPlugin());
	//Effects
	plugin_mgr.add_plugin(new EqualizerPlugin());
	plugin_mgr.add_plugin(new CompressorPlugin());
	plugin_mgr.add_plugin(new ReverbPlugin());
	plugin_mgr.add_plugin(new DelayPlugin());
	plugin_mgr.add_plugin(new ChorusPlugin());
	plugin_mgr.add_plugin(new FlangerPlugin());
	plugin_mgr.add_plugin(new PhaserPlugin());
	plugin_mgr.add_plugin(new WahWahPlugin());
	plugin_mgr.add_plugin(new TremoloPlugin());
	plugin_mgr.add_plugin(new BitCrusherPlugin());
	plugin_mgr.add_plugin(new AmplifierSimulationPlugin());
	plugin_mgr.add_plugin(new RotarySpeakerPlugin());
	plugin_mgr.add_plugin(new VocoderPlugin());
	plugin_mgr.add_plugin(new LooperPlugin());
	//Sequencers
	plugin_mgr.add_plugin(new ArpeggiatorPlugin());
	//Default engines
	engine.channels[0].scenes[0].active = true;

	for (size_t i = 0; i < engine.channels.size(); ++i) {
		engine.channels[i].scenes[0].source.input = 1;
	}
	std::cout << "Loaded engines" << std::endl;
	//Load programs
	prog_mgr.lock();
	prog_mgr.init_user(this);
	prog_mgr.load_all(&plugin_mgr);
	prog_mgr.apply_program(0, 0);
	prog_mgr.unlock();
	std::cout << "Loaded programs" << std::endl;

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

void MidiCube::process(double& lsample, double& rsample, double* inputs, const size_t input_count, SampleInfo& info) {
	if (lock.try_lock()) {
		//Messages
		size_t i = 0;
		for (MidiCubeInput in : this->inputs) {
			MidiMessage msg;
			while (in.in->read(&msg)) {
				process_midi(msg, i);
			}
			++i;
		}
		//Process
		engine.process_sample(lsample, rsample, inputs, input_count, info);
		lock.unlock();
	}
}

std::vector<MidiCubeInput> MidiCube::get_inputs() {
	return inputs;
}

inline void MidiCube::process_midi(MidiMessage& message, size_t input) {
	SampleInfo info = audio_handler.sample_info();
	size_t s = std::min((size_t) SOUND_ENGINE_MIDI_CHANNELS, used_sources);
	std::cout << message.channel << std::endl;
	//Sources
	for (size_t i = 0; i < s; ++i) {
		MidiSource& source = sources[i];
		if (source.device >= 0 && static_cast<unsigned int>(source.device) == input && (source.channel < 0 || static_cast<unsigned int>(source.channel) == message.channel)) {
			//Filter
			bool pass = true;
			switch (message.type) {
			case CONTROL_CHANGE:
				pass = source.transfer_cc;
				break;
			case PROGRAM_CHANGE:
				pass = source.transfer_prog_change;
				break;
			case PITCH_BEND:
				pass = source.transfer_pitch_bend;
				break;
			default:
				break;
			}
			//Type
			if (pass) {
				engine.send(message, i, source, info);
			}
		}
	}
}

MidiCube::~MidiCube() {
	lock.lock();
	audio_handler.close();
	//Save programs
	prog_mgr.lock();
	prog_mgr.save_all();
	prog_mgr.unlock();
	for (MidiCubeInput in : inputs) {
		delete in.in;
	}
	inputs.clear();
	lock.unlock();
}

void MidiCube::save_program(Program *prog) {
	engine.save_program(prog);
	std::cout << "Saved program: " << prog->name << std::endl;
}

void MidiCube::apply_program(Program *prog) {
	engine.apply_program(prog);
	std::cout << "Selected program: " << prog->name << std::endl;
}

void MidiCube::notify_property_update(void *source, void *prop) {
	property_callback(source, prop);
}
