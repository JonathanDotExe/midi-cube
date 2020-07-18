/*
 * synthesizer.cpp
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#include "synthesizer.h"

Synthesizer::Synthesizer() {
	preset = new SynthesizerPreset();

	release_time = 0;
	for (size_t i = 0; i < preset->oscilators; ++i) {
		if (preset->oscilators[i].env.release > release_time) {
			release_time = preset->oscilators[i].env.release;
		}
	}
}

void Synthesizer::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note) {

}

void Synthesizer::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {

}

void Synthesizer::control_change(unsigned int control, unsigned int value) {
	//Control bindings
	for (size_t i = 0; i < preset->control_bindings.size() ; ++i) {
		OscilatorSlot* slot = preset->oscilators.at(preset->control_bindings[i].oscilator).osc;
		double val = preset->control_bindings[i].from + (preset->control_bindings[i].to - preset->control_bindings[i].from) * value/127.0;
		slot->set_property(preset->control_bindings[i].property, val);
	}
}

bool Synthesizer::note_finished(SampleInfo& info, TriggeredNote& note) {
	return !note.pressed && note.release_time + release_time < info.time;
}

std::string get_name() {
	return "Synthesizer";
}


Synthesizer::~Synthesizer() {
	delete preset;
	preset = nullptr;
}

