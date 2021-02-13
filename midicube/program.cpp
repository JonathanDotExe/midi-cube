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

Bank* load_bank(std::string path) {
	return new Bank{"Default"};	//TODO
}

void save_bank(std::string path) {
	//TODO
}


ProgramManager::ProgramManager(std::string path) {
	this->path = path;
}

void ProgramManager::load_all() {
	std::regex reg(".*\\.json");
	for (const auto& f : boost::filesystem::directory_iterator(path)) {
		std::string file = f.path().string();
		if (std::regex_match(file, reg)) {
			Bank* bank = load_bank(file);
			if (bank) {
				if (banks.insert(banks.end(), std::pair<std::string, Bank*>(f.path().stem().string(), bank))) {
					std::cout << "Preset with filename " << f.path().stem().string() << " already exists!" << std::endl;
				}
			}
		}
	}
	if (banks.size() == 0) {
		Bank* bank = new Bank();
		bank->name = "Default";
		bank->programs.push_back(new Program{"Init"});
	}
}

void ProgramManager::save_all() {
	for (auto b : banks) {

	}
}

ProgramManager::~ProgramManager() {
	for (auto b : banks) {
		delete b.second;
	}
	banks.clear();
}
