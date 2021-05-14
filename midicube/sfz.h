/*
 * sfz.h
 *
 *  Created on: Apr 30, 2021
 *      Author: jojo
 */

#include <string>
#include <vector>
#include <unordered_map>

struct SfzRegion {
	std::unordered_map<std::string, std::string> opcodes;
};

struct SfzGroup {
	std::unordered_map<std::string, std::string> opcodes;
	std::vector<SfzRegion> regions;
};

struct SfzInstrument {
	std::unordered_map<std::string, std::string> global;
	std::vector<SfzGroup> groups;
};


class SfzParser {

public:

	SfzInstrument parse(std::string text);

	~SfzParser() {

	}

};

