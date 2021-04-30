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

enum ParserMode {
	GLOBAL, GROUP, REGION
};

SfzInstrument SfzParser::parse(std::string text) {
	SfzInstrument instrument;
	boost::replace_all(text, "\r", "");
	std::vector<std::string> lines = {};
	boost::split(lines, text, boost::is_any_of("\n"));

	std::vector<std::string> tokens = {};
	//Remove comments
	for (std::string line : lines) {
		if (line.rfind("//", 0) != 0) {
			std::vector<std::string> t = {};
			boost::split(t, line, boost::is_any_of(" "));
			for (std::string token : t) {
				tokens.push_back(token);
			}
		}
	}

	//Parse lines
	for (std::string token : tokens) {
		boost::trim(token);
		if (token.size() != 0) {
			//TODO
			std::cout << token << std::endl;
		}
	}

	return instrument;
}
