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

Bank* load_bank(std::string path);

class ProgramManager {
private:
	std::string path;
	size_t curr_bank = 0;
	size_t curr_program = 0;
public:
	std::vector<Bank*> banks;
	ProgramManager(std::string path);
	void load_all();
	void save_all();
	~ProgramManager();
};

#endif /* MIDICUBE_PROGRAM_H_ */
