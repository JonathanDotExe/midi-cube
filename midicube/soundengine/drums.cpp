/*
 * drums.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */


#include "drums.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


//SampleDrums
SampleDrums::SampleDrums () {
	drumkit = load_drumkit("./data/drumkits/drums1");
}

void SampleDrums::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	try {
		if (drumkit->notes.find(note.note) != drumkit->notes.end()) {
			AudioSample& audio = drumkit->notes[note.note];
			for (size_t i = 0; i < channels.size(); ++i) {
				channels[i] += audio.sample(i, info.time - note.start_time, info.sample_rate);
			}
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		throw e;
	}
}

bool SampleDrums::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
	if (drumkit->notes.find(note.note) != drumkit->notes.end()) {
		return info.time - note.start_time > (double) drumkit->notes[note.note].duration();
	}
	return true;
}

SampleDrums::~SampleDrums() {
	delete drumkit;
	drumkit = nullptr;
}

extern SampleDrumKit* load_drumkit(std::string folder) {
	//Load file
	std::ifstream def_file;
	def_file.open(folder + "/drumkit.json");
	if (def_file.is_open()) {
		//Parse json
		std::string json_text ((std::istreambuf_iterator<char>(def_file)), std::istreambuf_iterator<char>());
		auto j = json::parse(json_text);
		//Parse
		SampleDrumKit* drumkit = new SampleDrumKit();
		try {
			//Load sounds
			for (auto r : j.at("sounds").get<std::vector<json>>()) {
				unsigned int index = r.at("note").get<unsigned int>();
				drumkit->notes[index] = {};
				std::string file = folder + "/" + r.at("file").get<std::string>();
				if (!read_audio_file(drumkit->notes.at(index), file)) {
					std::cerr << "Couldn't load drum sample " << file << std::endl;
					throw std::runtime_error("Couldn't load drum sample " + file);
				}
			}
		}
		catch (json::exception& e) {
			delete drumkit;
			drumkit = nullptr;
			throw e;
		}
		catch (std::exception& e) {
			delete drumkit;
			drumkit = nullptr;
			throw e;
		}
		return drumkit;
	}
	else {
		std::cerr << "Couldn't load drumkit" << std::endl;
	}
	return nullptr;
}

template<>
std::string get_engine_name<SampleDrums>() {
	return "Sample Drums";
}

void __fix_link_drums_name__ () {
	get_engine_name<SampleDrums>();
}
