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
#include <fstream>
#include <streambuf>

unsigned int parse_sfz_note(std::string text) {
	unsigned int note = 0;
	boost::to_lower(text);

	size_t len = 1;
	bool found = true;
	if (text.rfind("c", 0) == 0) {
		note = 0;
	}
	else if (text.rfind("c#", 0) == 0 || text.rfind("db", 0) == 0) {
		len = 2;
		note = 1;
	}
	else if (text.rfind("d", 0) == 0) {
		note = 2;
	}
	else if (text.rfind("d#", 0) == 0 || text.rfind("eb", 0) == 0) {
		len = 2;
		note = 3;
	}
	else if (text.rfind("e", 0) == 0) {
		note = 4;
	}
	else if (text.rfind("f", 0) == 0) {
		note = 5;
	}
	else if (text.rfind("f#", 0) == 0 || text.rfind("gb", 0) == 0) {
		len = 2;
		note = 6;
	}
	else if (text.rfind("g", 0) == 0) {
		note = 7;
	}
	else if (text.rfind("g#", 0) == 0 || text.rfind("ab", 0) == 0) {
		len = 2;
		note = 8;
	}
	else if (text.rfind("a", 0) == 0) {
		note = 9;
	}
	else if (text.rfind("a#", 0) == 0 || text.rfind("bb", 0) == 0) {
		len = 2;
		note = 10;
	}
	else if (text.rfind("b", 0) == 0) {
		note = 11;
	}
	else {
		found = false;
	}

	if (found) {
		//Octave
		note += 12 * (std::stoi(text.substr(len)) + 1);
	}
	else {
		note = std::stoi(text);
	}

	return note;
}

enum ParserMode {
	NONE, GLOBAL, GROUP, REGION
};

SfzInstrument SfzParser::parse(std::string text, std::string path) {
	SfzInstrument instrument;
	boost::replace_all(text, "\r", "");
	std::vector<std::string> lines = {};
	boost::split(lines, text, boost::is_any_of("\n"));

	std::vector<std::string> tokens = {};
	std::unordered_map<std::string, std::string> defines;
	//Remove comments
	size_t i = 0;
	for (std::string line : lines) {
		if (line.rfind("//", 0) != 0) {
			//Apply defines
			for (auto pair : defines) {
				boost::replace_all(line, pair.first, pair.second);
			}
			std::vector<std::string> t = {};
			boost::split(t, line, boost::is_any_of(" "));
			//Define
			if (t.size() >= 3 && t[0] == "#define") {
				defines[t[1]] = t[2];
			}
			//Include
			else if (t.size() >= 2 && t[0] == "#include") {
				std::string file = t[1];
				for (size_t i = 2; i < t.size(); ++i) {
					file += " " + t[i];
				}
				boost::replace_all(file, "\"", "");

				//Load file
				std::string filename = path + "/" + file;
				std::fstream f(filename);
				std::string t;
				while (getline(f, t)) {
					lines.insert(lines.begin() + i + 1, t);
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
