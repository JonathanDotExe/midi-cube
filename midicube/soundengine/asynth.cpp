/*
 * asynth.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: jojo
 */
#include "asynth.h"


AnalogSynth::AnalogSynth() {
	preset.oscilators.at(0).volume = 1;
	preset.oscilators.at(0).env = {0.0005, 0, 1, 0.003};
	preset.oscilators.at(0).bank.unison_amount = 2;
}

void AnalogSynth::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	//Synthesize
	double sample = 0;
	for (size_t i = 0; i < preset.oscilators.size(); ++i) {
		OscilatorEntity& osc = preset.oscilators[i];
		if (osc.volume) {
			sample += oscilators.signal(note.freq, info.time_step, note_index + i * SOUND_ENGINE_POLYPHONY, osc.osc, osc.bank, false) * osc.volume;
		}
	}

	//Play
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i] += sample;
	}
}

void AnalogSynth::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status) {

}

void AnalogSynth::control_change(unsigned int control, unsigned int value) {

}

bool AnalogSynth::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
	bool finished = true;
	for (size_t i = 0; i < preset.oscilators.size() && finished; ++i) {
		OscilatorEntity& osc = preset.oscilators[i];
		if (osc.volume) {
			finished = osc.env.is_finished(info.time, note, env);
		}
	}
	return finished;
}

AnalogSynth::~AnalogSynth() {

}

template<>
std::string get_engine_name<AnalogSynth>() {
	return "Analog Synthesizer";
}

