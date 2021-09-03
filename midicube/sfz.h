/*
 * sfz.h
 *
 *  Created on: Apr 30, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_SFZ_H_
#define MIDICUBE_SFZ_H_

#include <string>
#include <vector>
#include <unordered_map>



struct ControlTrigger {
	double min_val = 0;
	double max_val = 1;
};

struct SfzRegion {
	std::unordered_map<std::string, std::string> opcodes;
};

struct SfzGroup {
	std::unordered_map<std::string, std::string> opcodes;
	std::vector<SfzRegion> regions;
};

struct SfzInstrument {
	std::unordered_map<std::string, std::string> global;
	std::unordered_map<std::string, std::string> control;
	std::vector<SfzGroup> groups;
};

unsigned int parse_sfz_note(std::string text);

class SfzParser {

public:

	SfzInstrument parse(std::vector<std::string> lines, std::string path);

	~SfzParser() {

	}

};

void convert_sfz_to_sampler(std::string src, std::string folder, std::string dst, std::string name);

#endif
