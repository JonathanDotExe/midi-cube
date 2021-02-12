/*
 * program.cpp
 *
 *  Created on: Feb 12, 2021
 *      Author: jojo
 */

#include "program.h"
#include <boost/filesystem.hpp>
#include <regex>

Bank* load_bank(std::string path, std::string filename) {
	return new Bank{"Default"};	//TODO
}

Bank* save_bank(std::string path) {
	return new Bank{"Default"};	//TODO
}


ProgramManager::ProgramManager(std::string path) {
	this->path = path;
}

void ProgramManager::load_all() {
	std::regex reg(".*\\.json");
	for (const auto& f : boost::filesystem::directory_iterator(path)) {
		std::string file = f.path().string();
		if (std::regex_match(file, reg)) {
			Bank* bank = load_bank(path, f.path().filename().string());
			if (bank) {
				banks.push_back(bank);
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
	for (Bank* bank : banks) {

	}
}

ProgramManager::~ProgramManager() {

}
