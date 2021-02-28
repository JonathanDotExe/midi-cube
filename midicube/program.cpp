/*
 * program.cpp
 *
 *  Created on: Feb 12, 2021
 *      Author: jojo
 */

#include "program.h"
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
	//Channels
	const auto& channels = tree.get_child_optional("channels");
	if (channels) {
		size_t i = 0;
		for (pt::ptree::value_type& c : channels.get()) {
			if (i < program->channels.size()) {
				//Channel
				program->channels[i].engine_index = c.second.get<ssize_t>("engine", -1);
				const auto& scenes = c.second.get_child_optional("scenes");
				if (scenes) {
					size_t j = 0;
					for (pt::ptree::value_type& s : scenes.get()) {
						if (j >= SOUND_ENGINE_SCENE_AMOUNT) {
							break;
						}
						program->channels[i].active[j] = s.second.get_value(false);
					}
					++j;
				}
				else {
					program->channels[i].active =  {c.second.get<bool>("active", false)};
				}
				program->channels[i].volume = c.second.get<double>("volume", 1);
				program->channels[i].panning = c.second.get<double>("panning", 0.5);
				//Source
				program->channels[i].source.input = c.second.get<ssize_t>("source.input", 1);
				program->channels[i].source.channel = c.second.get<unsigned int>("source.channel", 0);
				program->channels[i].source.start_note = c.second.get<unsigned int>("source.start_note", 0);
				program->channels[i].source.end_note = c.second.get<unsigned int>("source.end_note", 127);
				program->channels[i].source.start_velocity = c.second.get<unsigned int>("source.start_velocity", 0);
				program->channels[i].source.end_velocity = c.second.get<unsigned int>("source.end_velocity", 127);
				program->channels[i].source.octave = c.second.get<int>("source.octave", 0);
				program->channels[i].source.transfer_channel_aftertouch = c.second.get<bool>("source.transfer_channel_aftertouch", true);
				program->channels[i].source.transfer_pitch_bend = c.second.get<bool>("source.transfer_pitch_bend", true);
				program->channels[i].source.transfer_cc = c.second.get<bool>("source.transfer_cc", true);
				program->channels[i].source.transfer_prog_change = c.second.get<bool>("source.transfer_prog_change", true);
				program->channels[i].source.transfer_other = c.second.get<bool>("source.transfer_other", true);

				//Arpeggiator
				program->channels[i].arp_on = c.second.get<bool>("arpeggiator.on", false);
				program->channels[i].arpeggiator_bpm = c.second.get<unsigned int>("arpeggiator.bpm", 120);
				program->channels[i].arpeggiator.pattern = static_cast<ArpeggiatorPattern>(c.second.get<size_t>("arpeggiator.pattern", 0));
				program->channels[i].arpeggiator.octaves = c.second.get<unsigned int>("arpeggiator.octaves", 1);
				program->channels[i].arpeggiator.value = c.second.get<unsigned int>("arpeggiator.note_value", 1);
				program->channels[i].arpeggiator.hold = c.second.get<bool>("arpeggiator.hold", false);

				//Sound engine
				//FIXME
				//Organ
				if (program->channels[i].engine_index == 1) {
					program->channels[i].engine_program = new B3OrganProgram();
				}
				//Synth
				else if (program->channels[i].engine_index == 2) {
					program->channels[i].engine_program = new AnalogSynthProgram();
				}
				const auto& preset =  c.second.get_child_optional("preset");
				if (preset && program->channels[i].engine_program) {
					program->channels[i].engine_program->load(preset.get());
				}
			}
			++i;
		}
	}
	return program;
}

void save_program(Program* program, pt::ptree& tree) {
	tree.put("name", program->name);
	tree.put("metronome_bpm", program->metronome_bpm);
	//Channels
	for (size_t i = 0; i < program->channels.size(); ++i) {
		pt::ptree c;
		//Channel
		c.put("engine", program->channels[i].engine_index);
		for (size_t j = 0; j < SOUND_ENGINE_SCENE_AMOUNT; ++j) {
			c.add("scenes.active", program->channels[i].active[j]);
		}
		c.put("volume", program->channels[i].volume);
		c.put("panning", program->channels[i].panning);
		//Source
		c.put("source.input", program->channels[i].source.input);
		c.put("source.channel", program->channels[i].source.channel);
		c.put("source.start_note", program->channels[i].source.start_note);
		c.put("source.end_note", program->channels[i].source.end_note);
		c.put("source.start_velocity", program->channels[i].source.start_velocity);
		c.put("source.end_velocity", program->channels[i].source.end_velocity);
		c.put("source.octave", program->channels[i].source.octave);
		c.put("source.transfer_channel_aftertouch", program->channels[i].source.transfer_channel_aftertouch);
		c.put("source.transfer_pitch_bend", program->channels[i].source.transfer_pitch_bend);
		c.put("source.transfer_cc", program->channels[i].source.transfer_cc);
		c.put("source.transfer_prog_change", program->channels[i].source.transfer_prog_change);
		c.put("source.transfer_other", program->channels[i].source.transfer_other);
		//Arpeggiator
		c.put("arpeggiator.on", program->channels[i].arp_on);
		c.put("arpeggiator.bpm", program->channels[i].arpeggiator_bpm);
		c.put("arpeggiator.pattern", static_cast<size_t>(program->channels[i].arpeggiator.pattern));
		c.put("arpeggiator.octaves", program->channels[i].arpeggiator.octaves);
		c.put("arpeggiator.note_value", program->channels[i].arpeggiator.value);
		c.put("arpeggiator.hold", program->channels[i].arpeggiator.hold);
		//Sound Engine
		if (program->channels[i].engine_program) {
			pt::ptree preset = program->channels[i].engine_program->save();
			c.add_child("preset", preset);
		}


		tree.add_child("channels.channel", c);
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
			Program* prog = load_program(p.second);
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
	Bank* b = get_bank(bank);
	Program* prog = b->programs.at(program);
	user->apply_program(prog);
	curr_bank = bank;
	curr_program = program;
	program_name = prog->name;
	bank_name = b->name;

	//submit_change(ProgramManagerProperty::pProgramManagerBank, (int) curr_bank);
	//submit_change(ProgramManagerProperty::pProgramManagerProgram, (int) curr_program);
}

void ProgramManager::delete_program() {
	Bank* bank = get_curr_bank();
	bank->programs.erase(bank->programs.begin() + curr_program);
	if (bank->programs.size() == 0) {
		bank->programs.push_back(new Program{"Init"});
	}
	if (curr_program >= bank->programs.size()) {
		curr_program--;
	}
	apply_program(curr_bank, curr_program);
}

void ProgramManager::save_new_program() {
	Bank* bank = get_curr_bank();
	Program* prog = new Program();
	prog->name = program_name;
	user->save_program(prog);
	bank->programs.push_back(prog);
}

void ProgramManager::overwrite_program() {
	Program* prog = get_bank(curr_bank)->programs.at(curr_program);
	prog->name = program_name;
	user->save_program(prog);
}

void ProgramManager::save_new_bank() {
	Bank* bank = new Bank();
	bank->name = bank_name;
	bank->filename = bank_filename(bank_name);
	bank->programs.push_back(new Program{"Init"});
	//TODO check if already exists
	banks.push_back(bank);
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
			Bank* bank = load_bank(file, name);
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
	lock();
	for (auto b : banks) {
		save_bank(*b, path + "/" + b->filename + ".xml");
	}
	unlock();
}

ProgramManager::~ProgramManager() {
	lock();
	for (auto b : banks) {
		delete b;
	}
	banks.clear();
	unlock();
}

/*
void ProgramManager::set(size_t prop, PropertyValue value, size_t sub_prop) {
	switch ((ProgramManagerProperty) prop) {
	case ProgramManagerProperty::pProgramManagerBank:
		apply_program(value.ival, 0);
		break;
	case ProgramManagerProperty::pProgramManagerProgram:
		apply_program(curr_bank, value.ival);
		break;
	}
}

void ProgramManager::update_properties() {
	submit_change(ProgramManagerProperty::pProgramManagerBank, (int) curr_bank);
	submit_change(ProgramManagerProperty::pProgramManagerProgram, (int) curr_program);
}

PropertyValue ProgramManager::get(size_t prop, size_t sub_prop) {
	PropertyValue val;
	switch ((ProgramManagerProperty) prop) {
	case ProgramManagerProperty::pProgramManagerBank:
		val.ival = curr_bank;
		break;
	case ProgramManagerProperty::pProgramManagerProgram:
		val.ival = curr_program;
		break;
	}
	return val;
}*/
