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

#include "midiio.h"
#include "data.h"
#include "soundengine.h"

namespace pt = boost::property_tree;

#define PROGRAM_NAME_LENGTH 20
#define BANK_NAME_LENGTH 20

struct Program : public Copyable {
	std::string name;
	std::string host;
	pt::ptree data;

	Program(std::string name) : Copyable() {
		this->name = name;
	}

};

struct Bank {
	std::string name = "";
	std::string filename = "";
	bool preset = false;
	std::vector<Program*> programs;
	~Bank() {
		std::remove_if(programs.begin(), programs.end(), [](Program* p) {
			delete p;
			return true;
		});
	}
};

class ProgramUser {
public:
	//Methods are executed in realtime thread
	virtual void apply_program(Program* prog) = 0;
	virtual void save_program(Program* prog) = 0;
	virtual ~ProgramUser() {

	}
};

enum ProgramManagerProperty {
	pProgramManagerBank,
	pProgramManagerProgram,
};

class VersionManager {
private:
	const std::vector<std::function<void(pt::ptree&)>> versions;

public:
	const std::string host;

	VersionManager(std::string h, std::vector<std::function<void(pt::ptree&)>> v) : host(h), versions(v) {

	}

	unsigned int get_version();

	void update(pt::ptree& tree, size_t version);

};

class ProgramManager {
private:
	std::string path = "";
	std::string index_path = "";
	std::string preset_path = "";

	std::string host;
	VersionManager& version_manager;
	size_t curr_bank = 0;
	size_t curr_program = 0;
	std::vector<Bank*> banks;
	ProgramUser* user = nullptr;
	SpinLock mutex;

public:
	std::string bank_name = "";
	std::string program_name = "";

	ProgramManager(VersionManager& mgr, std::string path, std::string index_path, std::string preset_path);

	inline void lock() {
		mutex.lock();
	}
	inline void unlock() {
		mutex.unlock();
	}
	Program* load_program(pt::ptree& tree);

	void save_program(Program* program, pt::ptree& tree);

	Bank* load_bank(std::string path, std::string filename);

	void save_bank(Bank& bank, std::string path);
	//Mutex has to be locked by user
	size_t bank_count() {
		return banks.size();
	}
	//Mutex has to be locked by user
	void apply_program(size_t bank, size_t program);
	//Mutex has to be locked by user
	bool delete_program();
	//Mutex has to be locked by user
	bool save_new_program();
	//Mutex has to be locked by user
	bool save_init_program();
	//Mutex has to be locked by user
	bool overwrite_program();
	//Mutex has to be locked by user
	void save_new_bank();
	//Mutex has to be locked by user
	bool overwrite_bank();

	//Mutex has to be locked by user
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
	Program* get_curr_program() {
		Bank* bank = get_bank(curr_bank);
		return bank ? bank->programs.at(curr_program) : nullptr;
	}

	//Mutex has to be locked by user
	size_t get_curr_bank_index() {
		return curr_bank;
	}
	//Mutex has to be locked by user
	size_t get_curr_program_index() {
		return curr_program;
	}

	//Mutex has to be locked by user
	void load_all(PluginManager* mgr);
	//Mutex has to be locked by user
	void save_all();
	~ProgramManager();
};

#endif /* MIDICUBE_PROGRAM_H_ */
