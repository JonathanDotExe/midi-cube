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
#include <string>

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
	Program* program = new Program("");
	program->name = tree.get<std::string>("name", "Init");
	program->host = tree.get<std::string>("host", "");
	program->version = tree.get<unsigned int>("version", 0);
	program->data = tree;

	//Channels
	/*const auto& channels = tree.get_child_optional("channels");
	if (channels) {
		size_t i = 0;
		for (pt::ptree::value_type& c : channels.get()) {
			if (i < program->channels.size()) {
				//Channel
				if (c.second.get_child_optional("engine")) {
					program->channels[i].engine_program.load(c.second.get_child("engine"), mgr);
				}
				//Sequencer
				const auto& sequencers = c.second.get_child_optional("sequencers");
				if (sequencers) {
					size_t j = 0;
					for (pt::ptree::value_type& s : sequencers.get()) {
						if (j >= CHANNEL_SEQUENCER_AMOUNT) {
							break;
						}
						program->channels[i].sequencers[j].load(s.second, mgr);
						++j;
					}
				}
				program->channels[i].polyphony_limit = c.second.get<size_t>("polyphony_limit", 0);
				program->channels[i].input = c.second.get<ssize_t>("input", 0);
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
						program->channels[i].scenes[j].source.start_note = s.second.get<unsigned int>("source.start_note", 0);
						program->channels[i].scenes[j].source.end_note = s.second.get<unsigned int>("source.end_note", 127);
						program->channels[i].scenes[j].source.start_velocity = s.second.get<unsigned int>("source.start_velocity", 0);
						program->channels[i].scenes[j].source.end_velocity = s.second.get<unsigned int>("source.end_velocity", 127);
						program->channels[i].scenes[j].source.octave = s.second.get<int>("source.octave", 0);
						program->channels[i].scenes[j].source.update_channel = s.second.get<int>("source.update_channel", 0);

						++j;
					}
				}

				program->channels[i].volume.load(c.second, "volume", 0.5);
				program->channels[i].panning.load(c.second, "panning", 0);
				program->channels[i].redirect.channel = c.second.get<int>("redirect.channel", -1);
				program->channels[i].redirect.redirect_to = c.second.get<int>("redirect.redirect_to", -1);
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
		std::cout << "Found effects" << std::endl;
		size_t j = 0;
		for (pt::ptree::value_type& s : effects.get()) {
			if (j >= SOUND_ENGINE_MASTER_EFFECT_AMOUNT) {
				break;
			}
			program->effects[j].next_effect = s.second.get<ssize_t>("next_effect", -1);
			const auto& effect = s.second.get_child_optional("effect");
			if (effect) {
				program->effects[j].prog.load(effect.get(), mgr);
			}
			++j;
		}
	}*/
	return program;
}

void save_program(Program* program, pt::ptree& tree) {
	tree = program->data;
	tree.put("name", program->name);
	tree.put("host", program->host);
	tree.put("version", program->version);
	//Channels
	/*for (size_t i = 0; i < program->channels.size(); ++i) {
		pt::ptree c;
		ChannelProgram default_channel;
		if (!program->channels[i].is_default()) {
			//Channel
			c.put_child("engine", program->channels[i].engine_program.save());
			//Effects
			for (size_t j = 0; j < CHANNEL_SEQUENCER_AMOUNT; ++j) {
				PluginSlotProgram& seq = program->channels[i].sequencers[j];
				c.add_child("sequencers.sequencers", seq.save());
			}
			c.put("polyphony_limit", program->channels[i].polyphony_limit);
			c.put("input", program->channels[i].input);
			for (size_t j = 0; j < SOUND_ENGINE_SCENE_AMOUNT; ++j) {
				pt::ptree s;

				SoundEngineScene default_scene;
				if (!program->channels[i].scenes[j].is_default()) {
					s.put("active", program->channels[i].scenes[j].active);
					s.put("sustain", program->channels[i].scenes[j].sustain);
					s.put("pitch_bend", program->channels[i].scenes[j].pitch_bend);
					//Source
					s.put("source.start_note", program->channels[i].scenes[j].source.start_note);
					s.put("source.end_note", program->channels[i].scenes[j].source.end_note);
					s.put("source.start_velocity", program->channels[i].scenes[j].source.start_velocity);
					s.put("source.end_velocity", program->channels[i].scenes[j].source.end_velocity);
					s.put("source.octave", program->channels[i].scenes[j].source.octave);
					s.put("source.update_channel", program->channels[i].scenes[j].source.update_channel);
				}

				c.add_child("scenes.scene", s);
			}
			program->channels[i].volume.save(c, "volume");
			program->channels[i].panning.save(c, "panning");
			c.put("redirect.channel", program->channels[i].redirect.channel);
			c.put("redirect.redirect_to", program->channels[i].redirect.redirect_to);

			c.put("send_master", program->channels[i].send_master);
			//Effects
			for (size_t j = 0; j < CHANNEL_INSERT_EFFECT_AMOUNT; ++j) {
				PluginSlotProgram& effect = program->channels[i].effects[j];
				c.add_child("insert_effects.effect", effect.save());
			}
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
	}*/
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
	std::cout << "Saved programs" << std::endl;
	//Write
	try {
		pt::write_xml(path, tree);
	}
	catch (pt::xml_parser_error& e) {
		std::cerr << "Couldn't save bank " << path << std::endl;
	}
}

ProgramManager::ProgramManager(VersionManager& mgr, std::string path, std::string index_path, std::string preset_path) : version_manager(mgr){
	this->path = path;
	this->index_path = index_path;
	this->preset_path = preset_path;
}

void ProgramManager::apply_program(size_t bank, size_t program) {
	Bank* b = get_bank(bank);
	Program* prog = b->programs.at(program);
	user->apply_program(prog);
	curr_bank = bank;
	curr_program = program;
	program_name = prog->name;
	bank_name = b->name;
}

bool ProgramManager::delete_program() {
	//Delete
	Bank* bank = get_curr_bank();
	if (!bank->preset) {
		bank->programs.erase(bank->programs.begin() + curr_program);
		if (bank->programs.size() == 0) {
			bank->programs.push_back(new Program("Init"));
		}
		if (curr_program >= bank->programs.size()) {
			curr_program--;
		}

		//Apply
		Program* prog = bank->programs.at(curr_program);
		user->apply_program(prog);
		program_name = prog->name;
		return true;
	}
	return false;
}

bool ProgramManager::save_new_program() {
	Bank* bank = get_curr_bank();
	if (!bank->preset) {
		Program* prog = new Program("");
		prog->name = program_name;

		user->save_program(prog);
		bank->programs.push_back(prog);
		curr_program = bank->programs.size() - 1;

		save_all();
		return true;
	}
	return false;
}

bool ProgramManager::save_init_program() {
	Bank* bank = get_curr_bank();
	if (!bank->preset) {
		Program* prog = new Program("");
		prog->name = program_name;

		user->apply_program(prog);
		bank->programs.push_back(prog);
		curr_program = bank->programs.size() - 1;
		save_all();
		return true;
	}
	return false;
}

bool ProgramManager::overwrite_program() {
	Bank* bank = get_bank(curr_bank);
	if (!bank->preset) {
		Program* prog = get_bank(curr_bank)->programs.at(curr_program);

		prog->name = program_name;
		user->save_program(prog);
		save_all();
		return true;
	}
	return false;
}

void ProgramManager::save_new_bank() {
	Bank* bank = new Bank();
	bank->name = bank_name;
	bank->filename = bank_filename(bank_name);
	bank->programs.push_back(new Program("Init"));
	if (std::any_of(banks.begin(), banks.end(), [bank](Bank* b){ return b->filename == bank->filename; })) {
		delete bank;
	}
	else {
		banks.push_back(bank);
	}
}

bool ProgramManager::overwrite_bank() {
	Bank* bank = get_bank(curr_bank);
	if (!bank->preset) {
		bank->name = bank_name;
		//Don't update filename
		return true;
	}
	return false;
}

void ProgramManager::load_all(PluginManager* mgr) {
	//Load presets
	std::vector<std::string> presets = {"preset_piano", "preset_synth_pad", "preset_synth_lead", "preset_synth_bass", "preset_synth_keys", "preset_organ", "preset_combi", "preset_special"};
	for (std::string filename : presets) {
		Bank* bank = load_bank(preset_path + "/" + filename + ".xml", filename, mgr);
		if (bank) {
			bank->preset = true;
			banks.push_back(bank);
			std::cout << "Loaded preset bank " << bank->name << std::endl;
		}
	}
	//Load index
	pt::ptree tree;
	std::vector<std::string> index = {};
	try {
		pt::read_xml(index_path, tree);
		if (tree.get_child_optional("index")) {
			for (auto child : tree.get_child("index")) {
				index.push_back(child.second.get_value<std::string>());
			}
		}
	}
	catch (pt::xml_parser_error& e) {
		std::cerr << "Couldn't load program index " << path << std::endl;
	}
	//Load banks
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
		bank->programs.push_back(new Program("Init"));
		banks.push_back(bank);
	}
	//Reorder
	size_t i = presets.size();
	for (std::string name : index) {
		//Find bank
		for (size_t j = i; j < banks.size(); ++j) {
			if (name == banks[j]->filename) {
				//Swap
				Bank* bank = banks[j];
				banks.erase(banks.begin() + j);
				banks.insert(banks.begin() + i, bank);
				++i;
				break;
			}
		}
	}
}

void ProgramManager::save_all() {
	//Save programs
	std::vector<std::string> filenames{};
	for (Bank* b : banks) {
		if (!b->preset) {
			while (std::find(filenames.begin(), filenames.end(), b->filename) != filenames.end()) {
				b->filename += "_";
			}
			save_bank(*b, path + "/" + b->filename + ".xml");
			filenames.push_back(b->filename);
		}
	}
	//Save index
	pt::ptree tree;
	for (std::string name : filenames) {
		tree.add("index.filename", name);
	}
	//Write
	try {
		pt::write_xml(index_path, tree);
	}
	catch (pt::xml_parser_error& e) {
		std::cerr << "Couldn't save bank " << path << std::endl;
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

