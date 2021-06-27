/*
 * program.h
 *
 *  Created on: Feb 12, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_PROGRAM_H_
#define MIDICUBE_PROGRAM_H_

#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <functional>
#include <map>
#include <mutex>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "midi.h"
#include "data.h"
#include "soundengine/soundengine.h"

namespace pt = boost::property_tree;

#define PROGRAM_NAME_LENGTH 20
#define BANK_NAME_LENGTH 20

struct Bank {
	std::string name = "";
	std::string filename = "";
	std::vector<Program*> programs;
	~Bank() {
		std::remove_if(programs.begin(), programs.end(), [](Program* p) {
			delete p;
			return true;
		});
	}
};

Program* load_program(pt::ptree& tree, std::vector<EffectBuilder*>);

void save_program(Program* program, pt::ptree& tree);

Bank* load_bank(std::string path, std::string filename, std::vector<EffectBuilder*> builders);

void save_bank(Bank& bank, std::string path);

class ProgramUser {
public:
	//Methods are executed in realtime thread
	virtual void apply_program(Program* prog) = 0;
	virtual void save_program(Program* prog) = 0;
	virtual std::vector<EffectBuilder*> get_effect_builders() = 0;
	virtual ~ProgramUser() {

	}
};

enum ProgramManagerProperty {
	pProgramManagerBank,
	pProgramManagerProgram,
};

class ProgramManager {
private:
	std::string path;
	size_t curr_bank = 0;
	size_t curr_program = 0;
	std::vector<Bank*> banks;
	ActionHandler& handler;
	ProgramUser* user = nullptr;

public:
	std::string bank_name = "";
	std::string program_name = "";

	ProgramManager(std::string path, ActionHandler& h);

	//Mutex has to be locked by user
	size_t bank_count() {
		return banks.size();
	}

	void apply_program(size_t bank, size_t program);
	//Audio thread only
	void apply_program_direct(size_t bank, size_t program);
	void delete_program();
	void save_new_program();
	void save_init_program();
	void overwrite_program();
	//Audio thread only
	void save_new_bank();
	//Audio thread only
	void overwrite_bank();

	//On intializiation
	bool init_user(ProgramUser* user) {
		bool success = false;
		if (!this->user) {
			this->user = user;
			success = true;
		}
		return success;
	}
	//Mutex has to be locked by user
	Bank* get_bank(size_t bank) {
		return banks.at(bank);
	}
	//Mutex has to be locked by user
	Bank* get_curr_bank() {
		return get_bank(curr_bank);
	}
	//Mutex has to be locked by user
	size_t get_curr_bank_index() {
		return curr_bank;
	}
	//Mutex has to be locked by user
	size_t get_curr_program_index() {
		return curr_program;
	}

	void load_all();
	void save_all();
	~ProgramManager();
};

#endif /* MIDICUBE_PROGRAM_H_ */
