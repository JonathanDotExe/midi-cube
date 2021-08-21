/*
 * sfz.cpp
 *
 *  Created on: Apr 30, 2021
 *      Author: jojo
 */

#include "sfz.h"
#include "synthesis.h"
#include "filter.h"
#include <boost/algorithm/string.hpp>
#include <regex>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <functional>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace pt = boost::property_tree;

unsigned int parse_sfz_note(std::string text) {
	unsigned int note = 0;
	boost::to_lower(text);
	boost::replace_all(text, "c#", "db");
	boost::replace_all(text, "d#", "eb");
	boost::replace_all(text, "f#", "gb");
	boost::replace_all(text, "g#", "ab");
	boost::replace_all(text, "a#", "bb");

	size_t len = 1;
	bool found = true;
	if (text.rfind("c", 0) == 0) {
		note = 0;
	}
	else if (text.rfind("db", 0) == 0) {
		len = 2;
		note = 1;
	}
	else if (text.rfind("d", 0) == 0) {
		note = 2;
	}
	else if (text.rfind("eb", 0) == 0) {
		len = 2;
		note = 3;
	}
	else if (text.rfind("e", 0) == 0) {
		note = 4;
	}
	else if (text.rfind("f", 0) == 0) {
		note = 5;
	}
	else if (text.rfind("gb", 0) == 0) {
		len = 2;
		note = 6;
	}
	else if (text.rfind("g", 0) == 0) {
		note = 7;
	}
	else if (text.rfind("ab", 0) == 0) {
		len = 2;
		note = 8;
	}
	else if (text.rfind("a", 0) == 0) {
		note = 9;
	}
	else if (text.rfind("bb", 0) == 0) {
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
	NONE, GLOBAL, CONTROL, GROUP, REGION
};

SfzInstrument SfzParser::parse(std::vector<std::string> lines, std::string path) {
	SfzInstrument instrument;

	std::vector<std::string> tokens = {};
	std::unordered_map<std::string, std::string> defines;
	//Remove comments
	for (size_t i = 0; i < lines.size(); ++i) {
		std::string line = lines[i];
		if (line.rfind("//", 0) != 0) {
			//Apply defines
			if (line.rfind("#define ", 0) != 0) {
				for (auto pair : defines) {
					boost::replace_all(line, pair.first, pair.second);
				}
			}

			std::vector<std::string> t = {};
			boost::split(t, line, boost::is_any_of(" "));
			//Define
			if (t.size() >= 3 && t[0] == "#define") {
				defines[t[1]] = t[2];
			}
			//Include
			else if (t.size() >= 2 && t[0] == "#include") {
				std::cout << "include" << std::endl;
				std::string file = t[1];
				for (size_t j = 2; j < t.size(); ++j) {
					file += " " + t[j];
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
				bool chain = false;
				std::string ts = "";
				//Add tokens
				for (std::string token : t) {
					size_t index = token.find("=");
					if (index != std::string::npos) {
						if (chain) {
							tokens.push_back(ts);
							ts = "";
						}
						chain = true;
						ts += token;
					}
					else if (chain) {
						ts += " " + token;
					}
					else {
						tokens.push_back(token);
					}
				}
				if (chain) {
					tokens.push_back(ts);
				}
			}
		}
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
			else if (token == "<control>") {
				mode = CONTROL;
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
					case CONTROL:
						instrument.control[opcode] = value;
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

static bool parse_modulatable(std::pair<std::string, std::string> opcode, std::string name, std::string converted_name, pt::ptree& tree, std::string vel_name="", std::function<double (std::string)> value_converter = [](std::string str) { return std::stod(str); }, std::function<double (std::string)> mod_converter = [](std::string str) { return std::stod(str); }, bool cc_multiplier = false) {
	if (opcode.first == name) {
		tree.put(converted_name + ".value", value_converter(opcode.second));
		return true;
	}
	else if (opcode.first.rfind(name + "_oncc", 0) == 0) {
		int cc = std::stoi(opcode.first.substr(name.size() + std::string("_oncc").size()));
		tree.put(converted_name + ".cc", cc);
		if (cc_multiplier) {
			tree.put(converted_name + ".cc_multiplier", mod_converter(opcode.second));
		}
		else {
			tree.put(converted_name + ".cc_amount", mod_converter(opcode.second));
		}
		return true;
	}
	else if (opcode.first == vel_name) {
		tree.put(converted_name + ".velocity_amount", mod_converter(opcode.second));
	}
	return false;
}

static void parse_opcodes(std::unordered_map<std::string, std::string> opcodes, pt::ptree& tree) {
	std::function<double(std::string)> filter_conv = [](std::string str) {
		return invert_scale_cutoff(std::stod(str));
	};
	std::function<double(std::string)> filter_mod_conv = [](std::string str) {
		return (invert_scale_cutoff(200) - invert_scale_cutoff(100)) * std::stod(str)/1200.0;
	};
	std::function<double(std::string)> db_conv = [](std::string str) {
		return db_to_amp(std::stod(str));
	};
	//Opcodes
	for (auto opcode : opcodes) {
		try {
			if (opcode.first == "amp_veltrack") {
				tree.put("envelope.velocity_amount", std::stod(opcode.second)/100.0);
			}
			else if (parse_modulatable(opcode, "ampeg_attack", "envelope.attack", tree, "ampeg_vel2attack")) {

			 }
			else if (parse_modulatable(opcode, "ampeg_decay", "envelope.decay", tree, "ampeg_vel2decay")) {

			}
			else if (parse_modulatable(opcode, "ampeg_sustain", "envelope.sustain", tree, "ampeg_vel2sustain")) {

			}
			else if (parse_modulatable(opcode, "ampeg_release", "envelope.release", tree, "ampeg_vel2release")) {

			}
			else if (opcode.first == "lokey") {
				tree.put("min_note", parse_sfz_note(opcode.second));
			}
			else if (opcode.first == "hikey") {
				tree.put("max_note", parse_sfz_note(opcode.second));
			}
			else if (opcode.first == "pitch_keycenter") {
				tree.put("note", parse_sfz_note(opcode.second));
			}
			else if (opcode.first == "lovel") {
				tree.put("min_velocity", std::stoi(opcode.second));
			}
			else if (opcode.first == "hivel") {
				tree.put("max_velocity", std::stoi(opcode.second));
			}
			else if (parse_modulatable(opcode, "volume", "volume", tree, "", db_conv, db_conv, true) || parse_modulatable(opcode, "gain", "volume", tree, "", db_conv, db_conv, true) || parse_modulatable(opcode, "group_volume", "volume", tree, "", db_conv, db_conv, true)) {

			}
			else if (opcode.first == "pitch_keytrack") {
				tree.put("pitch_keytrack", std::stod(opcode.second)/100.0);
			}
			else if (opcode.first == "rt_decay") {
				tree.put("release_decay", db_to_amp(-std::stod(opcode.second)));
			}
			else if (opcode.first == "trigger") {
				tree.put("trigger", opcode.second);
				if (opcode.second != "attack") {
					tree.put("envelope.sustain_entire_sample", true);
				}
			}
			else if (opcode.first == "sample") {
				std::string path = opcode.second;
				boost::replace_all(path, "\\", "/");
				tree.put("sample.name", path);
			}
			else if (opcode.first == "loop_mode") {
				if (opcode.second == "one_shot") {
					tree.put("envelope.sustain_entire_sample", true);
					tree.put("loop_type", "no_loop");
				}
				else if (opcode.second == "no_loop") {
					tree.put("loop_type", "no_loop");
				}
				else if (opcode.second == "loop_continuous") {
					tree.put("loop_type", "attack_loop");
				}
			}
			else if (opcode.first == "loop_start") {
				tree.put("sample.loop_start", std::stoi(opcode.second));
			}
			else if (opcode.first == "loop_end") {
				tree.put("sample.loop_end", std::stoi(opcode.second));
			}
			else if (opcode.first == "offset") {
				tree.put("sample.start", std::stoi(opcode.second));
			}
			else if (parse_modulatable(opcode, "ampeg_hold", "envelope.attack_hold", tree, "ampeg_vel2hold")) {

			}
			//Filter
			else if (parse_modulatable(opcode, "cutoff", "filter.cutoff", tree, "", filter_conv, filter_mod_conv)) {

			}
			//TODO tune
			else {
				std::cout << "Skipping unrecognized opcode " << opcode.first << "="
						<< opcode.second << std::endl;
			}
		}
		catch (std::invalid_argument& e) {
			std::cout << "Couldn't convert opcode " << opcode.first << "="
									<< opcode.second << std::endl;
		}
	}
}

static void parse_control_opcodes(std::unordered_map<std::string, std::string> opcodes, pt::ptree& tree) {
	for (auto opcode : opcodes) {
		if (opcode.first == "default_path") {
			tree.put("default_path", opcode.second);
		}
	}
}

void convert_sfz_to_sampler(std::string src, std::string dst, std::string name) {
	SfzParser parser;
	std::fstream f(src);
	std::vector<std::string> lines;
	std::string t;

	while (std::getline(f, t)) {
		lines.push_back(t);
	}

	SfzInstrument instrument = parser.parse(lines, dst);
	std::cout << "Loaded sfz instrument " << name << std::endl;
	pt::ptree tree;
	pt::ptree sound;
	//Name
	sound.put("name", name);
	parse_opcodes(instrument.global, sound);
	parse_control_opcodes(instrument.control, sound);

	//Groups
	for (SfzGroup group : instrument.groups) {
		pt::ptree g;
		parse_opcodes(group.opcodes, g);
		//Regions
		for (SfzRegion region : group.regions) {
			pt::ptree r;
			parse_opcodes(region.opcodes, r);
			g.add_child("regions.region", r);
		}
		sound.add_child("groups.group", g);
	}
	tree.add_child("sound", sound);
	//Save to file
	try {
		pt::write_xml(dst, tree);
		std::cout << "Finished converting sound" << name << "!" << std::endl;
	}
	catch (pt::xml_parser_error& e) {
		std::cerr << "Couldn't save file " << dst << "!" << std::endl;
	}
}
