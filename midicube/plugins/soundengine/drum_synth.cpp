/*
 * drums.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */


#include "drum_synth.h"
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

//DrumSynth
DrumSynth::DrumSynth(PluginHost& h, Plugin& p) : SoundEngine(h, p){

}

void DrumSynth::process_note_sample(const SampleInfo& info, TriggeredNote& note, size_t note_index) {
	try {
		if (preset.notes.find(note.note) != preset.notes.end()) {
			AudioSample& audio = preset.notes[note.note];
			outputs[0] += audio.sample(0, info.time - note.start_time, info.sample_rate);
			outputs[1] += audio.sample(1, info.time - note.start_time, info.sample_rate);
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		throw e;
	}
}

bool DrumSynth::note_finished(const SampleInfo& info, TriggeredNote& note, size_t note_index) {
	if (preset.notes.find(note.note) != preset.notes.end()) {
		return info.time - note.start_time > (double) preset.notes[note.note].duration();
	}
	return true;
}

void DrumSynth::apply_program(PluginProgram *prog) {

}

void DrumSynth::save_program(PluginProgram **prog) {
	DrumSynthProgram* p = dynamic_cast<DrumSynthProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		*prog = new DrumSynthProgram();
	}
}

DrumSynth::~DrumSynth() {

}

//DrumSynthPlugin
PluginProgram* DrumSynthPlugin::create_program() {
	return new DrumSynthProgram();
}

PluginInstance* DrumSynthPlugin::create(PluginHost *host) {
	return new DrumSynth(*host, *this);
}

//DrumSynthProgram
void DrumSynthProgram::load(boost::property_tree::ptree tree) {
}

std::string DrumSynthProgram::get_plugin_name() {
	return DRUM_SYNTH_POLYPHONY;
}

boost::property_tree::ptree DrumSynthProgram::save() {
	return {};
}
