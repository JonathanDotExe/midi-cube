/*
 * program.cpp
 *
 *  Created on: Feb 12, 2021
 *      Author: jojo
 */

#include "program.h"
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

Program* load_program(pt::ptree& tree, PluginManager* mgr) {
	Program* program = new Program();
	program->name = tree.get<std::string>("name", "Init");
	program->metronome_bpm = tree.get<unsigned int>("metronome_bpm", 120);
	//Motion Sequencers
	/*const auto& motion_sequencers = tree.get_child_optional("motion_seqeuncers");
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
	}*/
	//Channels
	const auto& channels = tree.get_child_optional("channels");
	if (channels) {
		size_t i = 0;
		for (pt::ptree::value_type& c : channels.get()) {
			if (i < program->channels.size()) {
				//Channel
				if (c.second.get_child_optional("engine")) {
					program->channels[i].engine_program.load(c.second.get_child("engine"), mgr);
				}
				if (c.second.get_child_optional("sequencer")) {
					program->channels[i].sequencer_program.load(c.second.get_child("sequencer"), mgr);
				}
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

				//Sound engine
				program->channels[i].send_master = c.second.get<ssize_t>("send_master", -1);
				//Effects
				const auto& effects = c.second.get_child_optional("insert_effects");
				if (effects) {
					size_t j = 0;
					for (pt::ptree::value_type& s : effects.get()) {
						if (j >= CHANNEL_INSERT_EFFECT_AMOUNT) {
							break;
						}
						program->channels[i].effects[j].load(s.second, mgr);
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
			program->effects[j].prog.load(s.second, mgr);
			++j;
		}
	}
	return program;
}

void save_program(Program* program, pt::ptree& tree) {
	tree.put("name", program->name);
	tree.put("metronome_bpm", program->metronome_bpm);
	//Motion Sequencers
	/*for (size_t i = 0; i < program->motion_sequencers.size(); ++i) {
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
	}*/
	//Channels
	for (size_t i = 0; i < program->channels.size(); ++i) {
		pt::ptree c;
		//Channel
		c.put_child("engine", program->channels[i].engine_program.save());
		c.put_child("sequencer", program->channels[i].sequencer_program.save());
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

		c.put("send_master", program->channels[i].send_master);
		//Effects
		for (size_t j = 0; j < CHANNEL_INSERT_EFFECT_AMOUNT; ++j) {
			pt::ptree t;
			PluginSlotProgram& effect = program->channels[i].effects[j];
			t.put_child("effect", effect.save());
		}

		tree.add_child("channels.channel", c);
	}
	//Effects
	for (size_t i = 0; i < SOUND_ENGINE_MASTER_EFFECT_AMOUNT; ++i) {
		pt::ptree t;
		MasterEffectProgram& effect = program->effects[i];
		t.put_child("effect", effect.prog.save());
		t.put("next_effect", effect.next_effect);
		tree.add_child("master_effects.effect", t);
	}
}

Bank* load_bank(std::string path, std::string filename, PluginManager* mgr) {
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
			Program* prog = load_program(p.second, mgr);
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

ProgramManager::ProgramManager(std::string path) {
	this->path = path;
}

void ProgramManager::apply_program(size_t bank, size_t program) {
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
}

void ProgramManager::save_new_program() {
	lock();
	Bank* bank = get_curr_bank();
	Program* prog = new Program();
	prog->name = program_name;

	user->save_program(prog);
	bank->programs.push_back(prog);
	curr_program = bank->programs.size() - 1;

	save_all();
	unlock();
}

void ProgramManager::save_init_program() {
	lock();
	Bank* bank = get_curr_bank();
	Program* prog = new Program();
	prog->name = program_name;

	user->apply_program(prog);
	bank->programs.push_back(prog);
	curr_program = bank->programs.size() - 1;
	save_all();
	unlock();
}

void ProgramManager::overwrite_program() {
	lock();
	Program* prog = get_bank(curr_bank)->programs.at(curr_program);

	prog->name = program_name;
	user->save_program(prog);
	save_all();
	unlock();
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

void ProgramManager::load_all(PluginManager* mgr) {
	lock();
	boost::filesystem::create_directory(path);
	std::regex reg(".*\\.xml");
	for (const auto& f : boost::filesystem::directory_iterator(path)) {
		std::string file = f.path().string();
		if (std::regex_match(file, reg)) {
			std::string name = f.path().stem().string();
			Bank* bank = load_bank(file, name, mgr);
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

