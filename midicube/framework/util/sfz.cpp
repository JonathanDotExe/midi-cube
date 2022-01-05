/*
 * sfz.cpp
 *
 *  Created on: Apr 30, 2021
 *      Author: jojo
 */

#include "sfz.h"
#include "../dsp/synthesis.h"
#include "../dsp/filter.h"
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
	NONE, GLOBAL, CONTROL, MASTER, GROUP, REGION
};

SfzInstrument SfzParser::parse(std::vector<std::string> lines, std::string path) {
	SfzInstrument instrument;

	std::vector<std::string> tokens = {};
	std::unordered_map<std::string, std::string> defines;
	//Remove comments
	for (size_t i = 0; i < lines.size(); ++i) {
		std::string line = lines[i];
		if (line.rfind("//", 0) != 0 && !std::all_of(line.begin(), line.end(), isspace)) {
			//Apply defines
			if (line.rfind("#define ", 0) != 0) {
				for (auto pair : defines) {
					boost::replace_all(line, pair.first, pair.second);
				}
			}

			boost::replace_all(line, "\r", "");
			boost::replace_all(line, "\n", "");
			std::vector<std::string> t = {};
			boost::split(t, line, boost::is_any_of(" "));
			//Define
			if (t.size() >= 3 && t[0] == "#define") {
				defines[t[1]] = t[2];
				std::cout << "Applying define " << t[1] << " " << t[2] << std::endl;
			}
			//Include
			else if (t.size() >= 2 && t[0] == "#include") {
				std::string file = t[1];
				for (size_t j = 2; j < t.size(); ++j) {
					file += " " + t[j];
				}
				boost::replace_all(file, "\"", "");

				//Load file
				std::string filename = path + "/" + file;
				std::cout << "Including \"" << filename << "\"" << std::endl;
				std::ifstream f;
				f.open(filename);
				std::string t;
				if (f.fail()) {
					std::cout << strerror(errno) << std::endl;
				}
				size_t line_count = 1;
				while (getline(f, t)) {
					lines.insert(lines.begin() + i + line_count, t);
					++line_count;
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
			if (token == "<master>") {
				mode = MASTER;
				instrument.masters.push_back({});
			}
			else if (token == "<group>") {
				mode = GROUP;
				if (instrument.masters.size() == 0) {
					instrument.masters.push_back({});
				}
				instrument.masters.back().groups.push_back({});
			}
			else if (token == "<region>") {
				mode = REGION;
				if (instrument.masters.size() == 0) {
					instrument.masters.push_back({});
				}
				if (instrument.masters.back().groups.size() == 0) {
					instrument.masters.back().groups.push_back({});
				}
				instrument.masters.back().groups.back().regions.push_back({});
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
					case MASTER:
						instrument.masters.back().opcodes[opcode] = value;
						break;
					case GROUP:
						instrument.masters.back().groups.back().opcodes[opcode] = value;
						break;
					case REGION:
						instrument.masters.back().groups.back().regions.back().opcodes[opcode] = value;
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
		pt::ptree control;
		int cc = std::stoi(opcode.first.substr(name.size() + std::string("_oncc").size()));
		control.put("cc", cc);
		control.put("amount", mod_converter(opcode.second));
		control.put("multiply", cc_multiplier);
		//Amplitude FIXME
		if (converted_name == "amplitude" && !tree.get_child_optional(converted_name + ".value")) {
			tree.put(converted_name + ".value", 0.0);
		}
		tree.add_child(converted_name + ".cc.control", control);
		return true;
	}
	else if (vel_name != "" && opcode.first == vel_name) {
		tree.put(converted_name + ".velocity_amount", mod_converter(opcode.second));
		return true;
	}
	return false;
}

static void parse_opcodes(std::unordered_map<std::string, std::string> opcodes, pt::ptree& tree, std::unordered_map<unsigned int, std::string>& preset_names) {
	std::function<double(std::string)> filter_conv = [](std::string str) {
		return invert_scale_cutoff(std::stod(str));
	};
	std::function<double(std::string)> filter_mod_conv = [](std::string str) {
		return (invert_scale_cutoff(200) - invert_scale_cutoff(100)) * std::stod(str)/1200.0;
	};
	std::function<double(std::string)> db_conv = [](std::string str) {
		return db_to_amp(std::stod(str));
	};
	std::function<double(std::string)> percent_conv = [](std::string str) {
		return std::stod(str)/100.0;
	};
	std::unordered_map<unsigned int, ControlTrigger> triggers;
	unsigned int keyswitch_low = 0;
	unsigned int keyswitch_high = 0;
	std::string name = "";
	//Opcodes
	for (auto opcode : opcodes) {
		try {
			if (parse_modulatable(opcode, "amp_veltrack", "envelope.velocity_amount", tree, "", percent_conv, percent_conv)) {

			}
			else if (parse_modulatable(opcode, "ampeg_attack", "envelope.attack", tree, "ampeg_vel2attack")) {

			}
			else if (parse_modulatable(opcode, "ampeg_decay", "envelope.decay", tree, "ampeg_vel2decay")) {

			}
			else if (parse_modulatable(opcode, "ampeg_sustain", "envelope.sustain", tree, "ampeg_vel2sustain")) {

			}
			else if (parse_modulatable(opcode, "ampeg_release", "envelope.release", tree, "ampeg_vel2release")) {

			}
			else if (parse_modulatable(opcode, "tune", "pitch", tree, "", percent_conv, percent_conv) || parse_modulatable(opcode, "pitch", "pitch", tree, "", percent_conv, percent_conv)) {

			}
			else if (opcode.first == "sw_last") {
				unsigned int note = parse_sfz_note(opcode.second);
				tree.put("min_preset", note);
				tree.put("max_preset", note);
				keyswitch_low = note;
				keyswitch_high = note;
			}
			else if (opcode.first == "sw_lolast") {
				unsigned int note = parse_sfz_note(opcode.second);
				tree.put("min_preset", note);
				keyswitch_low = note;
			}
			else if (opcode.first == "sw_hilast") {
				unsigned int note = parse_sfz_note(opcode.second);
				tree.put("max_preset", note);
				keyswitch_high = note;
			}
			else if (opcode.first == "sw_lokey") { //FIXME only in global
				tree.put("preset_start", parse_sfz_note(opcode.second));
			}
			else if (opcode.first == "sw_hikey") { //FIXME only in global
				tree.put("preset_end", parse_sfz_note(opcode.second));
			}
			else if (opcode.first == "sw_label") {
				name = opcode.second;
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
			else if (opcode.first == "key") {
				tree.put("min_note", parse_sfz_note(opcode.second));
				tree.put("max_note", parse_sfz_note(opcode.second));
				tree.put("note", parse_sfz_note(opcode.second));
			}
			else if (opcode.first == "lovel") {
				tree.put("min_velocity", std::stoi(opcode.second));
			}
			else if (opcode.first == "hivel") {
				tree.put("max_velocity", std::stoi(opcode.second));
			}
			else if (opcode.first.rfind("locc", 0) == 0) {
				int cc = std::stoi(opcode.first.substr(std::string("locc").size()));
				if (!triggers.count(cc)) {
					triggers[cc] = {0, 127};
				}
				triggers[cc].min_val = std::stoi(opcode.second);
			}
			else if (opcode.first.rfind("hicc", 0) == 0) {
				int cc = std::stoi(opcode.first.substr(std::string("hicc").size()));
				if (!triggers.count(cc)) {
					triggers[cc] = {0, 127};
				}
				triggers[cc].max_val = std::stoi(opcode.second);
			}
			else if (parse_modulatable(opcode, "volume", "volume", tree, "", db_conv, db_conv, true) || parse_modulatable(opcode, "gain", "volume", tree, "", db_conv, db_conv, true) || parse_modulatable(opcode, "group_volume", "volume", tree, "", db_conv, db_conv, true)) {

			}
			else if (parse_modulatable(opcode, "amplitude", "amplitude", tree, "", percent_conv, percent_conv)) {

			}
			else if (parse_modulatable(opcode, "pan", "pan", tree, "", percent_conv, percent_conv)) {

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
			else if (parse_modulatable(opcode, "offset", "sample.start", tree, "")) {
				tree.put("sample.start", std::stoi(opcode.second));
			}
			else if (parse_modulatable(opcode, "ampeg_hold", "envelope.attack_hold", tree, "ampeg_vel2hold")) {

			}
			//Filter
			else if (parse_modulatable(opcode, "cutoff", "filter.cutoff", tree, "fil_veltrack", filter_conv, filter_mod_conv)) {

			}
			else if (opcode.first == "fil_keytrack") {
				tree.put("filter.kb_track", std::stoi(opcode.second)/100.0);
			}
			else if (opcode.first == "fil_keycenter") {
				tree.put("filter.kb_track_note", parse_sfz_note(opcode.second));
			}
			else if (opcode.first == "fil_type") {
				/* LP_12, LP_24, HP_12, HP_24, LP_12_BP, LP_24_BP, LP_6, HP_6, BP_12 */
				if (opcode.second == "lpf_1p") {
					tree.put("filter.type", "LP_6");
				}
				else if (opcode.second == "hpf_1p") {
					tree.put("filter.type", "HP_6");
				}
				else if (opcode.second == "lpf_2p") {
					tree.put("filter.type", "LP_12");
				}
				else if (opcode.second == "hpf_2p") {
					tree.put("filter.type", "HP_12");
				}
				else if (opcode.second == "lpf_4p") {
					tree.put("filter.type", "LP_24");
				}
				else if (opcode.second == "hpf_4p") {
					tree.put("filter.type", "HP_24");
				}
				else if (opcode.second == "bpf_2p") {
					tree.put("filter.type", "BP_12");
				}
				else if (opcode.second == "bpf_4p") {
					tree.put("filter.type", "LP_24_BP");
					std::cout << "Filter type bpf_4p is implemented differently and may sound unexpected!" << std::endl;
				}
				else {
					std::cout << "Skipping unrecognized filter type "
											<< opcode.second << std::endl;
				}
			}
			//Ignore
			else if (opcode.first == "group_label") {

			}
			else if (opcode.first == "region_label") {

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
	for (auto trigger : triggers) {
		pt::ptree t;
		t.put("cc", trigger.first);
		t.put("min_value", static_cast<unsigned int>(trigger.second.min_val));
		t.put("max_value", static_cast<unsigned int>(trigger.second.max_val));
		tree.add_child("control_triggers.control", t);
	}
	if (name != "") {
		for (unsigned int i = keyswitch_low; i <= keyswitch_high; ++i) {
			preset_names[i] = name;
		}
	}
}

static void parse_control_opcodes(std::unordered_map<std::string, std::string> opcodes, pt::ptree& tree) {
	std::unordered_map<unsigned int, pt::ptree> controls;
	for (auto opcode : opcodes) {
		if (opcode.first == "default_path") {
			tree.put("default_path", opcode.second);
		}
		else if (opcode.first.rfind("set_cc", 0) == 0) {
			unsigned int cc = std::stoi(opcode.first.substr(std::string("set_cc").size()));
			if (controls.find(cc) == controls.end()) {
				controls[cc] = {};
				controls[cc].put("cc", cc);
			}
			controls[cc].put("default_value", std::stoi(opcode.second)/127.0);
		}
		else if (opcode.first.rfind("set_hdcc", 0) == 0) {
			unsigned int cc = std::stoi(opcode.first.substr(std::string("sethd_cc").size()));
			if (controls.find(cc) == controls.end()) {
				controls[cc] = {};
				controls[cc].put("cc", cc);
			}
			controls[cc].put("default_value", std::stod(opcode.second));
		}
		else if (opcode.first.rfind("label_cc", 0) == 0) {
			unsigned int cc = std::stoi(opcode.first.substr(std::string("label_cc").size()));
			if (controls.find(cc) == controls.end()) {
				controls[cc] = {};
				controls[cc].put("cc", cc);
			}
			controls[cc].put("name", opcode.second);
			controls[cc].put("save", true);
		}
	}
	//Add controls
	for (auto control : controls) {
		tree.add_child("controls.control", control.second);
	}
}

void convert_sfz_to_sampler(std::string src, std::string folder, std::string dst, std::string name) {
	SfzParser parser;
	std::fstream f(src);
	std::vector<std::string> lines;
	std::string t;

	while (std::getline(f, t)) {
		lines.push_back(t);
	}

	SfzInstrument instrument = parser.parse(lines, folder);
	std::cout << "Loaded sfz instrument " << name << std::endl;
	pt::ptree tree;
	pt::ptree sound;
	//Name
	std::unordered_map<unsigned int, std::string> preset_names;
	sound.put("name", name);
	parse_opcodes(instrument.global, sound, preset_names);
	parse_control_opcodes(instrument.control, sound);

	//Groups
	for (SfzMaster master : instrument.masters) {
		pt::ptree m;
		parse_opcodes(master.opcodes, m, preset_names);
		//Groups
		for (SfzGroup group : master.groups) {
			pt::ptree g;
			parse_opcodes(group.opcodes, g, preset_names);
			//Regions
			for (SfzRegion region : group.regions) {
				pt::ptree r;
				parse_opcodes(region.opcodes, r, preset_names);
				g.add_child("regions.region", r);
			}
			m.add_child("groups.group", g);
		}
		sound.add_child("groups.group", m);
	}

	//Presets
	for (auto preset : preset_names) {
		pt::ptree p;
		p.put("name", preset.second);
		p.put("index", preset.first);
		sound.add_child("presets.preset", p);
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
