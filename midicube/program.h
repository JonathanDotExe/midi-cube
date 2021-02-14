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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "property.h"

namespace pt = boost::property_tree;

struct Program {
	std::string name;
};

struct Bank {
	std::string name;
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

class ProgramManager : public PropertyHolder {
private:
	std::string path;
	size_t curr_bank = 0;
	size_t curr_program = 0;
	std::map<std::string, Bank*> banks;
	ProgramUser* user = nullptr;
public:
	ProgramManager(std::string path);
	void apply_program(size_t bank, size_t program);
	bool init_user(ProgramUser* user) {
		if (!this->user) {
			this->user = user;
			apply_program(curr_bank, curr_program);
			return true;
		}
		return false;
	}
	void load_all();
	void save_all();
	~ProgramManager();
	virtual void set(size_t prop, PropertyValue value, size_t sub_prop);
	virtual void update_properties();
	virtual PropertyValue get(size_t prop, size_t sub_prop);
};

#endif /* MIDICUBE_PROGRAM_H_ */
