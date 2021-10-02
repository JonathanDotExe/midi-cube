/*
 * program.cpp
 *
 *  Created on: Feb 12, 2021
 *      Author: jojo
 */

#include "program.h"
#include "soundengine/sampler.h"
#include "soundengine/organ.h"
#include "soundengine/asynth.h"
#include <boost/filesystem.hpp>
#include <regex>
#include <iostream>
#include <cctype>

std::string bank_filename(std::string name) {
	std::string filename = "";
	for (char ch : name) {
		if (ch == ' ' || ch == '_') {
			filename += '_';
		}
		else if (isalpha(ch) || isdigit(ch)) {
			filename += (char) tolower(ch);
		}
	}
	return filename;
}

Program* load_program(pt::ptree& tree) {
	Program* program = new Program();
	program->name = tree.get<std::string>("name", "Init");
	program->metronome_bpm = tree.get<unsigned int>("metronome_bpm", 120);
	//Motion Sequencers
	const auto& motion_sequencers = tree.get_child_optional("motion_seqeuncers");
	if (motion_sequencers) {
		size_t i = 0;
		for (pt::ptree::value_type& m : motion_sequencers.get()) {
			if (i < program->motion_sequencers.size()) {
				program->motion_sequencers[i].clock_value = m.second.get("clock_value", 1);
				const auto& steps = m.second.get_child_optional("steps");
				size_t j = 0;
				for (pt::ptree::value_type& s : steps.get()) {
					if (j < program->motion_sequencers[i].entries.size()) {
						program->motion_sequencers[i].entries[j].beats = s.second.get("beats", 1);
						program->motion_sequencers[i].entries[j].value = s.second.get("value", 0.0);
						program->motion_sequencers[i].entries[j].shape = (ADSREnvelopeShape) s.second.get("shape", 0);
					}
					j++;
				}
			}
			++i;
		}
	}
	//Channels
	const auto& channels = tree.get_child_optional("channels");
	if (channels) {
		size_t i = 0;
		for (pt::ptree::value_type& c : channels.get()) {
			if (i < program->channels.size()) {
				//Channel
				program->channels[i].engine_index = c.second.get<ssize_t>("engine", -1);
				program->channels[i].polyphony_limit = c.second.get<size_t>("polyphony_limit", 0);
				const auto& scenes = c.second.get_child_optional("scenes");
				if (scenes) {
					size_t j = 0;
					for (pt::ptree::value_type& s : scenes.get()) {
						if (j >= SOUND_ENGINE_SCENE_AMOUNT) {
							break;
						}
						program->channels[i].scenes[j].active = s.second.get<bool>("active", false);
						program->channels[i].scenes[j].sustain = s.second.get<bool>("sustain", true);
						program->channels[i].scenes[j].pitch_bend = s.second.get<bool>("pitch_bend", true);
						//Source
						program->channels[i].scenes[j].source.input = s.second.get<ssize_t>("source.input", 1);
						program->channels[i].scenes[j].source.start_note = s.second.get<unsigned int>("source.start_note", 0);
						program->channels[i].scenes[j].source.end_note = s.second.get<unsigned int>("source.end_note", 127);
						program->channels[i].scenes[j].source.start_velocity = s.second.get<unsigned int>("source.start_velocity", 0);
						program->channels[i].scenes[j].source.end_velocity = s.second.get<unsigned int>("source.end_velocity", 127);
						program->channels[i].scenes[j].source.octave = s.second.get<int>("source.octave", 0);
						program->channels[i].scenes[j].source.transfer_channel_aftertouch = s.second.get<bool>("source.transfer_channel_aftertouch", true);
						program->channels[i].scenes[j].source.transfer_pitch_bend = s.second.get<bool>("source.transfer_pitch_bend", true);
						program->channels[i].scenes[j].source.transfer_cc = s.second.get<bool>("source.transfer_cc", true);
						program->channels[i].scenes[j].source.transfer_prog_change = s.second.get<bool>("source.transfer_prog_change", true);
						program->channels[i].scenes[j].source.transfer_other = s.second.get<bool>("source.transfer_other", true);

						++j;
					}
				}

				program->channels[i].volume.load(c.second, "volume", 0.5);
				program->channels[i].panning.load(c.second, "panning", 0);

				//Arpeggiator
				program->channels[i].arp_on = c.second.get<bool>("arpeggiator.on", false);
				program->channels[i].arpeggiator_bpm = c.second.get<unsigned int>("arpeggiator.bpm", 120);
				program->channels[i].arpeggiator.pattern = static_cast<ArpeggiatorPattern>(c.second.get<size_t>("arpeggiator.pattern", 0));
				program->channels[i].arpeggiator.octaves = c.second.get<unsigned int>("arpeggiator.octaves", 1);
				program->channels[i].arpeggiator.value = c.second.get<unsigned int>("arpeggiator.note_value", 1);
				program->channels[i].arpeggiator.hold = c.second.get<bool>("arpeggiator.hold", false);
				program->channels[i].arpeggiator.kb_sync = c.second.get<bool>("arpeggiator.kb_sync", true);
				program->channels[i].arpeggiator.repeat_edges = c.second.get<bool>("arpeggiator.repeat_edges", false);
				program->channels[i].arpeggiator.play_duplicates = c.second.get<bool>("arpeggiator.play_duplicates", false);
				program->channels[i].arpeggiator.master_sync = c.second.get<bool>("arpeggiator.master_sync", false);
				program->channels[i].arpeggiator.sustain = c.second.get<bool>("arpeggiator.sustain", false);

				//Sound engine
				//FIXME
				//Sampler
				if (program->channels[i].engine_index == 0) {
					program->channels[i].engine_program = new SamplerProgram();
				}
				//Organ
				else if (program->channels[i].engine_index == 1) {
					program->channels[i].engine_program = new B3OrganProgram();
				}
				//Synth
				else if (program->channels[i].engine_index == 2) {
					program->channels[i].engine_program = new AdavancedSynthProgram();
				}
				const auto& preset =  c.second.get_child_optional("preset");
				if (preset && program->channels[i].engine_program) {
					program->channels[i].engine_program->load(preset.get());
				}
				program->channels[i].send_master = c.second.get<ssize_t>("send_master", -1);
				//Effects
				const auto& effects = c.second.get_child_optional("insert_effects");
				if (effects) {
					size_t j = 0;
					for (pt::ptree::value_type& s : effects.get()) {
						if (j >= CHANNEL_INSERT_EFFECT_AMOUNT) {
							break;
						}
						program->channels[i].effects[j].effect = s.second.get<ssize_t>("effect_type", -1);
						const auto& p = s.second.get_child_optional("preset");
						if (p && program->channels[i].effects[j].effect >= 0 && program->channels[i].effects[j].effect < (ssize_t) builders.size()) {
							program->channels[i].effects[j].prog = builders.at(program->channels[i].effects[j].effect)->create_program();
							if (program->channels[i].effects[j].prog) {
								program->channels[i].effects[j].prog->load(p.get());
							}
						}
						++j;
					}
				}
			}
			++i;
		}
	}

	//Effects
	const auto& effects = tree.get_child_optional("master_effects");
	if (effects) {
		size_t j = 0;
		for (pt::ptree::value_type& s : effects.get()) {
			if (j >= SOUND_ENGINE_MASTER_EFFECT_AMOUNT) {
				break;
			}
			program->effects[j].effect = s.second.get<ssize_t>("effect_type", -1);
			program->effects[j].next_effect = s.second.get<ssize_t>("next_effect", -1);
			const auto& p = s.second.get_child_optional("preset");
			if (p && program->effects[j].effect >= 0 && program->effects[j].effect < (ssize_t) builders.size()) {
				program->effects[j].prog = builders.at(program->effects[j].effect)->create_program();
				std::cout << "Program " << program->effects[j].effect << std::endl;
				if (program->effects[j].prog) {
					std::cout << "Load" << std::endl;
					program->effects[j].prog->load(p.get());
				}
			}
			++j;
		}
	}
	return program;
}

void save_program(Program* program, pt::ptree& tree) {
	tree.put("name", program->name);
	tree.put("metronome_bpm", program->metronome_bpm);
	//Motion Sequencers
	for (size_t i = 0; i < program->motion_sequencers.size(); ++i) {
		pt::ptree m;
		m.put("clock_value", program->motion_sequencers[i].clock_value);
		for (size_t j = 0; j < program->motion_sequencers[i].entries.size(); ++j) {
			pt::ptree s;
			s.put("beats", program->motion_sequencers[i].entries[j].beats);
			s.put("shape", (int) program->motion_sequencers[i].entries[j].shape);
			s.put("value", program->motion_sequencers[i].entries[j].value);
			m.add_child("entry", s);
		}
		tree.add_child("entries", m);
	}
	//Channels
	for (size_t i = 0; i < program->channels.size(); ++i) {
		pt::ptree c;
		//Channel
		c.put("engine", program->channels[i].engine_index);
		c.put("polyphony_limit", program->channels[i].polyphony_limit);
		for (size_t j = 0; j < SOUND_ENGINE_SCENE_AMOUNT; ++j) {
			pt::ptree s;

			s.put("active", program->channels[i].scenes[j].active);
			s.put("sustain", program->channels[i].scenes[j].sustain);
			s.put("pitch_bend", program->channels[i].scenes[j].pitch_bend);
			//Source
			s.put("source.input", program->channels[i].scenes[j].source.input);
			s.put("source.start_note", program->channels[i].scenes[j].source.start_note);
			s.put("source.end_note", program->channels[i].scenes[j].source.end_note);
			s.put("source.start_velocity", program->channels[i].scenes[j].source.start_velocity);
			s.put("source.end_velocity", program->channels[i].scenes[j].source.end_velocity);
			s.put("source.octave", program->channels[i].scenes[j].source.octave);
			s.put("source.transfer_channel_aftertouch", program->channels[i].scenes[j].source.transfer_channel_aftertouch);
			s.put("source.transfer_pitch_bend", program->channels[i].scenes[j].source.transfer_pitch_bend);
			s.put("source.transfer_cc", program->channels[i].scenes[j].source.transfer_cc);
			s.put("source.transfer_prog_change", program->channels[i].scenes[j].source.transfer_prog_change);
			s.put("source.transfer_other", program->channels[i].scenes[j].source.transfer_other);

			c.add_child("scenes.scene", s);
		}
		program->channels[i].volume.save(c, "volume");
		program->channels[i].panning.save(c, "panning");
		//Arpeggiator
		c.put("arpeggiator.on", program->channels[i].arp_on);
		c.put("arpeggiator.bpm", program->channels[i].arpeggiator_bpm);
		c.put("arpeggiator.pattern", static_cast<size_t>(program->channels[i].arpeggiator.pattern));
		c.put("arpeggiator.octaves", program->channels[i].arpeggiator.octaves);
		c.put("arpeggiator.note_value", program->channels[i].arpeggiator.value);
		c.put("arpeggiator.hold", program->channels[i].arpeggiator.hold);
		c.put("arpeggiator.kb_sync", program->channels[i].arpeggiator.kb_sync);
		c.put("arpeggiator.repeat_edges", program->channels[i].arpeggiator.repeat_edges);
		c.put("arpeggiator.play_duplicates", program->channels[i].arpeggiator.play_duplicates);
		c.put("arpeggiator.master_sync", program->channels[i].arpeggiator.master_sync);
		c.put("arpeggiator.sustain", program->channels[i].arpeggiator.sustain);
		//Sound Engine
		if (program->channels[i].engine_program) {
			pt::ptree preset = program->channels[i].engine_program->save();
			c.add_child("preset", preset);
		}
		c.put("send_master", program->channels[i].send_master);
		//Effects
		for (size_t j = 0; j < CHANNEL_INSERT_EFFECT_AMOUNT; ++j) {
			pt::ptree t;
			InsertEffectProgram& effect = program->channels[i].effects[j];
			t.put("effect_type", effect.effect);
			if (effect.prog)  {
				t.add_child("preset", effect.prog->save());
			}
			c.add_child("insert_effects.effect", t);
		}


		tree.add_child("channels.channel", c);
	}
	//Effects
	for (size_t i = 0; i < SOUND_ENGINE_MASTER_EFFECT_AMOUNT; ++i) {
		pt::ptree t;
		MasterEffectProgram& effect = program->effects[i];
		t.put("effect_type", effect.effect);
		t.put("next_effect", effect.next_effect);
		if (effect.prog)  {
			t.add_child("preset", effect.prog->save());
		}
		tree.add_child("master_effects.effect", t);
	}
}

Bank* load_bank(std::string path, std::string filename) {
	pt::ptree tree;
	try {
		pt::read_xml(path, tree);
	}
	catch (pt::xml_parser_error& e) {
		std::cerr << "Couldn't load bank " << path << std::endl;
		return nullptr;
	}
	Bank* bank = new Bank();
	//Name
	bank->name = tree.get<std::string>("bank.name", filename);
	bank->filename = filename;
	//Programs
	const auto& programs =  tree.get_child_optional("bank.programs");
	if (programs) {
		for (pt::ptree::value_type& p : programs.get()) {
			Program* prog = load_program(p.second, builders);
			bank->programs.push_back(prog);
		}
	}
	return bank;
}

void save_bank(Bank& bank, std::string path) {
	pt::ptree tree;
	//Name
	tree.put("bank.name", bank.name);
	//Programs
	for (Program* program : bank.programs) {
		pt::ptree p;
		save_program(program, p);
		tree.add_child("bank.programs.program", p);
	}
	//Write
	try {
		pt::write_xml(path, tree);
	}
	catch (pt::xml_parser_error& e) {
		std::cerr << "Couldn't save bank " << path << std::endl;
	}
}


ProgramManager::ProgramManager(std::string path, ActionHandler& h) : handler(h) {
	this->path = path;
}

void ProgramManager::apply_program(size_t bank, size_t program) {
	handler.queue_action(new FunctionAction([this, bank, program]() {
		apply_program_direct(bank, program);
	}));
}

void ProgramManager::apply_program_direct(size_t bank, size_t program) {
	lock();
	Bank* b = get_bank(bank);
	Program* prog = b->programs.at(program);
	user->apply_program(prog);
	curr_bank = bank;
	curr_program = program;
	program_name = prog->name;
	bank_name = b->name;
	unlock();
}


void ProgramManager::delete_program() {
	handler.queue_action(new FunctionAction([this]() {
		lock();
		//Delete
		Bank* bank = get_curr_bank();
		bank->programs.erase(bank->programs.begin() + curr_program);
		if (bank->programs.size() == 0) {
			bank->programs.push_back(new Program{"Init"});
		}
		if (curr_program >= bank->programs.size()) {
			curr_program--;
		}

		//Apply
		Program* prog = bank->programs.at(curr_program);
		user->apply_program(prog);
		program_name = prog->name;
		unlock();
	}));
}

void ProgramManager::save_new_program() {
	handler.queue_action(new FunctionAction([this]() {
		lock();
		Bank* bank = get_curr_bank();
		Program* prog = new Program();
		prog->name = program_name;

		user->save_program(prog);
		bank->programs.push_back(prog);
		curr_program = bank->programs.size() - 1;

		save_all();
		unlock();
	}));
}

void ProgramManager::save_init_program() {
	handler.queue_action(new FunctionAction([this]() {
		lock();
		Bank* bank = get_curr_bank();
		Program* prog = new Program();
		prog->name = program_name;

		user->apply_program(prog);
		bank->programs.push_back(prog);
		curr_program = bank->programs.size() - 1;
		save_all();
		unlock();
	}));
}

void ProgramManager::overwrite_program() {
	handler.queue_action(new FunctionAction([this]() {
		lock();
		Program* prog = get_bank(curr_bank)->programs.at(curr_program);

		prog->name = program_name;
		user->save_program(prog);
		save_all();
		unlock();
	}));
}

void ProgramManager::save_new_bank() {
	Bank* bank = new Bank();
	bank->name = bank_name;
	bank->filename = bank_filename(bank_name);
	bank->programs.push_back(new Program{"Init"});
	if (std::any_of(banks.begin(), banks.end(), [bank](Bank* b){ return b->filename == bank->filename; })) {
		delete bank;
	}
	else {
		banks.push_back(bank);
	}
}

void ProgramManager::overwrite_bank() {
	Bank* bank = get_bank(curr_bank);
	bank->name = bank_name;
	//Don't update filename
}

void ProgramManager::load_all() {
	lock();
	boost::filesystem::create_directory(path);
	std::regex reg(".*\\.xml");
	for (const auto& f : boost::filesystem::directory_iterator(path)) {
		std::string file = f.path().string();
		if (std::regex_match(file, reg)) {
			std::string name = f.path().stem().string();
			Bank* bank = load_bank(file, name, user->get_effect_builders());
			if (bank) {
				banks.push_back(bank);
				std::cout << "Loaded bank " << bank->name << std::endl;
			}
		}
	}
	if (banks.size() == 0) {
		Bank* bank = new Bank();
		bank->name = "Default";
		bank->filename = "default";
		bank->programs.push_back(new Program{"Init"});
		banks.push_back(bank);
	}
	unlock();
}

void ProgramManager::save_all() {
	std::vector<std::string> filenames;
	for (auto b : banks) {
		while (std::find(filenames.begin(), filenames.end(), b->filename) != filenames.end()) {
			b->filename += "_";
		}
		save_bank(*b, path + "/" + b->filename + ".xml");
		filenames.push_back(b->filename);
	}
}

ProgramManager::~ProgramManager() {
	lock();
	for (auto b : banks) {
		delete b;
	}
	banks.clear();
	unlock();
}

