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

#include <boost/property_tree/xml_parser.hpp>

#define MIDICUBE_CONFIG_PATH "./data/config.xml"

static void process_func(double& lsample, double& rsample, double* inputs, const size_t input_count, SampleInfo& info, void* user_data) {
	((MidiCube*) user_data)->process(lsample, rsample, inputs, input_count, info);
}

MidiCube::MidiCube() : prog_mgr("./data/programs", "./data/program_index.xml", "./resources/presets") {
	audio_handler.set_sample_callback(&process_func, this);
}

void MidiCube::init() {
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
	//Load config
	try {
		pt::ptree tree;
		pt::read_xml(MIDICUBE_CONFIG_PATH, tree);
		if (tree.get_child_optional("config")) {
			std::cout << "Loading config" << std::endl;
			config.load(tree.get_child("config"));
		}
		else {
			std::cout << "Couldn't load config file" << std::endl;
		}
	}
	catch (pt::xml_parser_error& e) {
		std::cout << "Couldn't load config file" << std::endl;
	}

	//Default engines
	engine.channels[0].scenes[0].active = true;
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
	//Sources
	used_sources = std::min(config.default_sources.size(), (size_t) SOUND_ENGINE_MIDI_CHANNELS);
	for (size_t i = 0; i < used_sources; ++i) {
		sources[i] = config.default_sources[i];
	}
	//Init audio
	audio_handler.init(config.sample_rate, config.buffer_size, config.output_device, config.input_device, config.input_channels);
}

void MidiCube::process(double& lsample, double& rsample, double* inputs, const size_t input_count, SampleInfo& info) {
	if (lock.try_lock()) {
		//Actions
		action_handler.execute_realtime_actions();
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
	//Sources
	for (size_t i = 0; i < s; ++i) {
		MidiSource& source = sources[i];
		if (source.device >= 0 && static_cast<unsigned int>(source.device) == input && (source.channel < 0 || static_cast<unsigned int>(source.channel) == message.channel)) {
			//Filter
			bool pass = true;
			switch (message.type) {
			case CONTROL_CHANGE:
				pass = source.transfer_cc;
				if (pass && view) {
					pass = !view->on_cc(message.control(), message.value()/127.0);
				}
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
			//Send
			if (pass) {
				engine.send(message, i, source, info);
			}
		}
	}
}

const MidiCubeConfig& MidiCube::get_config() const {
	return config;
}

MidiCube::~MidiCube() {
	lock.lock();
	audio_handler.close();
	//Save programs
	prog_mgr.lock();
	prog_mgr.save_all();
	prog_mgr.unlock();
	//Save config
	try {
		pt::ptree tree;
		tree.put_child("config", config.save());
		pt::xml_writer_settings<std::string>settings ('\t', 1);
		pt::write_xml(MIDICUBE_CONFIG_PATH, tree, std::locale(), settings);
	}
	catch (pt::xml_parser_error& e) {
		std::cerr << "Couldn't save config !" << std::endl;
	}
	//Delete ins
	for (MidiCubeInput in : inputs) {
		delete in.in;
	}
	inputs.clear();
	lock.unlock();
	delete view;
}

void MidiCube::save_program(Program *prog) {
	lock.lock();
	engine.save_program(prog);
	lock.unlock();
	std::cout << "Saved program: " << prog->name << std::endl;
}

void MidiCube::apply_program(Program *prog) {
	lock.lock();
	engine.apply_program(prog);
	lock.unlock();
	std::cout << "Selected program: " << prog->name << std::endl;
}

void MidiCube::notify_property_update(void *source, void *prop) {
	if (property_callback) {
		property_callback(source, prop);
	}
}

PluginManager& MidiCube::get_plugin_manager() {
	return plugin_mgr;
}

ActionHandler& MidiCube::get_action_handler() {
	return action_handler;
}

void MidiCube::set_property_change_callback(std::function<void(void*, void*)> cb) {
	property_callback = cb;
}

void MidiCubeConfig::load(pt::ptree tree) {
	sample_rate = tree.get("sample_rate", sample_rate);
	buffer_size = tree.get("buffer_size", buffer_size);
	input_channels = tree.get("input_channels", input_channels);
	output_device = tree.get("output_device", output_device);
	input_device = tree.get("input_device", input_device);
	screen_sleep = tree.get("screen_sleep", screen_sleep);

	std::cout << "Buffer Size " << buffer_size << std::endl;

	//Sources
	if (tree.get_child_optional("default_sources")) {
		default_sources = {};
		for (auto s : tree.get_child("default_sources")) {
			MidiSource source;
			source.device = s.second.get("device", source.device);
			source.channel = s.second.get("channel", source.channel);
			source.transfer_cc = s.second.get("transfer_cc", source.transfer_cc);
			source.transfer_pitch_bend = s.second.get("transfer_pitch_bend", source.transfer_pitch_bend);
			source.transfer_prog_change = s.second.get("transfer_prog_change", source.transfer_prog_change);
			source.clock_in = s.second.get("clock_in", source.clock_in);
			default_sources.push_back(source);
		}
	}

	//Controls
	if (tree.get_child_optional("controls.banks")) {
		controls.control_banks = {};
		for (auto b : tree.get_child("controls.banks")) {
			ControlBank bank;
			if (b.second.get_child_optional("sliders")) {
				for (auto slider :b.second.get_child("sliders")) {
					bank.sliders.push_back(slider.second.get_value<unsigned int>(128));
				}
			}
			if (b.second.get_child_optional("knobs")) {
				for (auto knob : b.second.get_child("knobs")) {
					bank.knobs.push_back(knob.second.get_value<unsigned int>(128));
				}
			}
			if (b.second.get_child_optional("buttons")) {
				for (auto button : b.second.get_child("buttons")) {
					bank.buttons.push_back(button.second.get_value<unsigned int>(128));
				}
			}
			controls.control_banks.push_back(bank);
		}
		if (tree.get_child_optional("controls.scene_buttons")) {
			controls.scene_buttons = {};
			for (auto button : tree.get_child("controls.scene_buttons")) {
				controls.scene_buttons.push_back(button.second.get_value<unsigned int>(128));
			}
		}
		controls.mod_wheel = tree.get<unsigned int>("controls.mod_wheel", 128);
		controls.breath_controller = tree.get<unsigned int>("controls.breath_controller", 128);
		controls.volume_pedal = tree.get<unsigned int>("controls.volume_pedal", 128);
		controls.expresion_pedal = tree.get<unsigned int>("controls.expression_pedal", 128);
		controls.sustain_pedal = tree.get<unsigned int>("controls.sustain_pedal", 128);
		controls.sostenuto_pedal = tree.get<unsigned int>("controls.sostenuto_pedal", 128);
		controls.soft_pedal = tree.get<unsigned int>("controls.soft_pedal", 128);
	}
}

pt::ptree MidiCubeConfig::save() {
	pt::ptree tree;
	tree.put("sample_rate", sample_rate);
	tree.put("buffer_size", buffer_size);
	tree.put("input_channels", input_channels);
	tree.put("output_device", output_device);
	tree.put("input_device", input_device);
	tree.put("screen_sleep", screen_sleep);
	//Sources
	for (MidiSource source : default_sources) {
		pt::ptree s;
		s.put("device", source.device);
		s.put("channel", source.channel);
		s.put("transfer_cc", source.transfer_cc);
		s.put("transfer_pitch_bend", source.transfer_pitch_bend);
		s.put("transfer_prog_change", source.transfer_prog_change);
		s.put("clock_in", source.clock_in);
		tree.add_child("default_sources.source", s);
	}
	//Controls
	for (ControlBank& bank : controls.control_banks) {
		pt::ptree b;
		pt::ptree sliders;
		pt::ptree knobs;
		pt::ptree buttons;
		for (unsigned int slider : bank.sliders) {
			sliders.add("slider", slider);
		}
		for (unsigned int knob : bank.knobs) {
			knobs.add("knob", knob);
		}
		for (unsigned int button : bank.buttons) {
			buttons.add("button", button);
		}
		b.put_child("controls.sliders", sliders);
		b.put_child("controls.knobs", knobs);
		b.put_child("controls.buttons", buttons);

		tree.add_child("controls.banks", b);
	}
	for (unsigned int button : controls.scene_buttons) {
		tree.add("controls.scene_buttons.button", button);
	}
	tree.put("controls.mod_wheel", controls.mod_wheel);
	tree.put("controls.breath_controller", controls.breath_controller);
	tree.put("controls.volume_pedal", controls.volume_pedal);
	tree.put("controls.expression_pedal", controls.expresion_pedal);
	tree.put("controls.sustain_pedal", controls.sustain_pedal);
	tree.put("controls.sostenuto_pedal", controls.sostenuto_pedal);
	tree.put("controls.soft_pedal", controls.soft_pedal);


	return tree;
}

void MidiCube::copy_program() {
	Program* prog = new Program("");
	engine.save_program(prog);
	clipboard.copy(prog);
}

bool MidiCube::paste_program() {
	bool success = false;
	prog_mgr.lock();
	Program* prog = clipboard.paste<Program>();
	if (prog) {
		engine.apply_program(prog);
		success = true;
	}
	prog_mgr.unlock();
	return success;
}

void MidiCube::change_control_view(ControlView *view) {
	delete this->view;
	this->view = view;
}
