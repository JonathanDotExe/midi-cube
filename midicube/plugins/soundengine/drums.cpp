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
SampleDrums::SampleDrums(PluginHost& h, Plugin& p) : SoundEngine(h, p){
	drumkit = load_drumkit("./data/drumkits/drums1");
}

void SampleDrums::process_note_sample(const SampleInfo& info, Voice& note, size_t note_index) {
	try {
		if (drumkit->notes.find(note.note) != drumkit->notes.end()) {
			AudioSample& audio = drumkit->notes[note.note];
			outputs[0] += audio.sample(0, info.time - note.start_time, info.sample_rate);
			outputs[1] += audio.sample(1, info.time - note.start_time, info.sample_rate);
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		throw e;
	}
}

bool SampleDrums::note_finished(const SampleInfo& info, Voice& note, size_t note_index) {
	if (drumkit->notes.find(note.note) != drumkit->notes.end()) {
		return info.time - note.start_time > (double) drumkit->notes[note.note].duration();
	}
	return true;
}

void SampleDrums::apply_program(PluginProgram *prog) {

}

void SampleDrums::save_program(PluginProgram **prog) {
	SampleDrumsProgram* p = dynamic_cast<SampleDrumsProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		*prog = new SampleDrumsProgram();
	}
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
				if (!read_audio_file(drumkit->notes[index], file)) {
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


//SampleDrumsPlugin
PluginProgram* SampleDrumsPlugin::create_program() {
	return new SampleDrumsProgram();
}

PluginInstance* SampleDrumsPlugin::create(PluginHost *host) {
	return new SampleDrums(*host, *this);
}

//SampleDrumsProgram
void SampleDrumsProgram::load(boost::property_tree::ptree tree) {
}

std::string SampleDrumsProgram::get_plugin_name() {
	return SAMPLE_DRUMS_IDENTIFIER;
}

boost::property_tree::ptree SampleDrumsProgram::save() {
	return {};
}
