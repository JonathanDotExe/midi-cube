/*
 * drums.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */


#include "drums.h"
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

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

bool SampleDrums::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
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
	SampleDrumKit* drumkit = nullptr;
	pt::ptree tree;
	try {
		pt::read_json(folder + "/drumkit.json", tree);
		//Parse

		drumkit = new SampleDrumKit();
		try {
			//Load sounds
			for (auto r : tree.get_child("sounds")) {
				unsigned int index = r.second.get<unsigned int>("note", 0);
				drumkit->notes[index] = {};
				std::string file = folder + "/" + r.second.get<std::string>("file");
				if (!read_audio_file(drumkit->notes.at(index), file)) {
					std::cerr << "Couldn't load drum sample " << file << std::endl;
					throw std::runtime_error("Couldn't load drum sample " + file);
				}
			}
		}
		catch (std::exception& e) {
			delete drumkit;
			drumkit = nullptr;
			throw e;
		}
	}
	catch (pt::json_parser_error& e) {
		std::cerr << "Couldn't laod drumkit.json" << std::endl;
	}
	return drumkit;
}

template<>
std::string get_engine_name<SampleDrums>() {
	return "Sample Drums";
}

void __fix_link_drums_name__ () {
	get_engine_name<SampleDrums>();
}
