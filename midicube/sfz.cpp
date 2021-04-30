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
	boost::replace_all(text, "\n", " ");
	boost::replace_all(text, "\r", " ");
	std::vector<std::string> lines = {};
	boost::split(lines, text, boost::is_any_of(" "));

	//Parse lines
	for (std::string token : lines) {
		std::cout << token << std::endl;
	}

	return instrument;
}
