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
	for (size_t i = 0; i < preset->oscilators.size(); ++i) {
		if (preset->oscilators[i].env.release > release_time) {
			release_time = preset->oscilators[i].env.release;
		}
	}
}

void Synthesizer::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note) {
	//Envelopes
	std::vector<double> env_val;
	for (size_t i = 0; i < preset->envelopes.size(); ++i) {
		env_val.push_back(preset->envelopes[i].amplitude(info.time, note));
	}
	//Envelope Bindings
	for (size_t i = 0; i < preset->envelope_bindings.size(); ++i) {
		OscilatorSlot* osc = preset->oscilators.at(preset->envelope_bindings[i].oscilator).osc;
		double value = env_val.at(preset->envelope_bindings[i].envelope);
		double val = preset->envelope_bindings[i].from + (preset->envelope_bindings[i].to - preset->envelope_bindings[i].from) * value;
		osc->set_property(preset->envelope_bindings[i].property, val);
	}
	//Oscilator samples
	samples.clear();
	for (size_t i = 0; i < preset->oscilators.size(); ++i) {
		samples.push_back(preset->oscilators[i].osc->signal(info.time + note.phase_shift, note.freq) * preset->oscilators[i].env.amplitude(info.time, note));
	}
}

void Synthesizer::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {
	//Filters
	for (size_t i = 0; i < preset->filters.size(); ++i) {
		double filtered  = preset->filters[i].filter->apply(samples.at(preset->filters[i].osc), info.time_step);
		samples[preset->filters[i].osc] = filtered;
	}
	//Play samples
	for (size_t i = 0; i < channels.size(); ++i) {
		for (size_t j = 0; j < samples.size(); ++i) {
			channels[i] += samples[j];
		}
	}
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

