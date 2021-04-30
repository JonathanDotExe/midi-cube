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
	std::vector<std::string> lines = {};
	boost::split(lines, text, std::regex("/[ |\n]/"));

	//Parse lines
	for (std::string token : lines) {
		std::cout << token << std::endl;
	}

	return instrument;
}
