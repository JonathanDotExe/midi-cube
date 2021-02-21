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

Program* load_program(pt::ptree& tree) {
	Program* program = new Program();
	program->name = tree.get<std::string>("name");
	return program;
}

void save_program(Program* program, pt::ptree& tree) {
	tree.add("name", program->name);
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
	Program* prog = get_bank(bank)->programs.at(program);
	user->apply_program(prog);
	curr_bank = bank;
	curr_program = program;
	program_name = prog->name;

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
	//TODO check if already exists
	banks.push_back(bank);
}

void ProgramManager::overwrite_bank() {
	Bank* bank = new Bank();
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
