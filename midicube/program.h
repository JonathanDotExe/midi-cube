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

#include "property.h"

namespace pt = boost::property_tree;

struct Program {
	std::string name;
};

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

Program* load_program(pt::ptree& tree);

void save_program(Program* program, pt::ptree& tree);

Bank* load_bank(std::string path, std::string filename);

void save_bank(Bank& bank, std::string path);

class ProgramUser {
public:
	virtual void apply_program(Program* prog) = 0;
	virtual void save_program(Program* prog) = 0;
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
	ProgramUser* user = nullptr;
	std::string program_name = "";
	std::mutex mutex;

public:

	ProgramManager(std::string path);
	inline void lock() {
		mutex.lock();
	}
	inline void unlock() {
		mutex.unlock();
	}
	//Mutex has to be locked by user
	size_t bank_count() {
		return banks.size();
	}
	//Mutex has to be locked by user
	void apply_program(size_t bank, size_t program);

	void delete_program();

	void save_new_program();

	void overwrite_program();

	bool init_user(ProgramUser* user) {
		bool success = false;
		lock();
		if (!this->user) {
			this->user = user;
			apply_program(curr_bank, curr_program);
			success = true;
		}
		unlock();
		return success;
	}
	//User has to aquire mutex
	Bank* get_bank(size_t bank) {
		return banks.at(bank);
	}
	//User has to aquire mutex
	Bank* get_curr_bank() {
		return get_bank(curr_bank);
	}

	void load_all();
	void save_all();
	~ProgramManager();
};

#endif /* MIDICUBE_PROGRAM_H_ */
