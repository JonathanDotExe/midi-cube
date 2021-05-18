/*
 * sfz.cpp
 *
 *  Created on: Apr 30, 2021
 *      Author: jojo
 */

#include "sfz.h"
#include <boost/algorithm/string.hpp>
#include <regex>
#include <iostream>

const std::vector<std::string> notes{"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "h"};

unsigned int parse_sfz_note(std::string text) {
	unsigned int note = 0;
	boost::to_lower(text);

	size_t i = 0;
	std::string key = "";
	bool found = false;
	//Note values
	for (std::string n : notes) {
		if (text.rfind(n, 0) == 0) {
			note = i;
			key = n;
			found = true;
		}
		++i;
	}
	if (found) {
		//Octave
		note += 12 * (std::stoi(text.substr(key.size())) + 1);
	}
	else {
		note = std::stoi(text);
	}

	return note;
}

enum ParserMode {
	NONE, GLOBAL, GROUP, REGION
};

SfzInstrument SfzParser::parse(std::string text) {
	SfzInstrument instrument;
	boost::replace_all(text, "\r", "");
	std::vector<std::string> lines = {};
	boost::split(lines, text, boost::is_any_of("\n"));

	std::vector<std::string> tokens = {};
	//Remove comments
	size_t i = 0;
	for (std::string line : lines) {
		if (line.rfind("//", 0) != 0) {
			std::vector<std::string> t = {};
			boost::split(t, line, boost::is_any_of(" "));
			//Define
			if (t.size() >= 3 && t[0] == "#define") {
				//Replace
				for (size_t j = i + 1; j < lines.size(); ++j) {
					boost::replace_all(lines[j], t[1], t[2]);
				}
			}
			else {
				//Add tokens
				for (std::string token : t) {
					tokens.push_back(token);
				}
			}
		}
		++i;
	}

	//Parse lines
	ParserMode mode = NONE;
	for (std::string token : tokens) {
		boost::trim(token);
		if (token.size() != 0) {
			//Change header
			if (token == "<group>") {
				mode = GROUP;
				instrument.groups.push_back({});
			}
			else if (token == "<region>") {
				mode = REGION;
				if (instrument.groups.size() == 0) {
					instrument.groups.push_back({});
				}
				instrument.groups.at(instrument.groups.size() - 1).regions.push_back({});
			}
			else if (token == "<global>") {
				mode = GLOBAL;
			}
			//Attribute
			else {
				size_t index = token.find("=");
				if (index != std::string::npos) {
					std::string opcode = token.substr(0, index);
					std::string value = token.substr(index + 1);

					switch (mode) {
					case NONE:
						std::cout << "No header has been found yet. Ignoring opcode \"" << token << "\"." << std::endl;
						break;
					case GLOBAL:
						instrument.global[opcode] = value;
						break;
					case GROUP:
						instrument.groups.at(instrument.groups.size() - 1).opcodes[opcode] = value;
						break;
					case REGION:
						instrument.groups.at(instrument.groups.size() - 1).regions.at(instrument.groups.at(instrument.groups.size() - 1).regions.size() - 1).opcodes[opcode] = value;
						break;
					}
				}
				else {
					std::cout << "Ignoring unrecognized token \"" << token << "\"" << std::endl;
				}
			}
		}
	}

	return instrument;
}
