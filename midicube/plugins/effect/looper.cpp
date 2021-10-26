/*
 * looper.cpp
 *
 *  Created on: Sep 4, 2020
 *      Author: jojo
 */

#include "looper.h"
#include <cstddef>
#include <algorithm>
#include "../view/EffectView.h"

void LooperChannel::apply(double& lout, double& rout, const Metronome& metronome, const SampleInfo& info) {
	//Play
	if (play) {
		if (reset) {
			lbuffer = {};
			rbuffer = {};
			reset = false;
		}

		unsigned int bpm = metronome.get_bpm();
		size_t index = info.sample_time % (preset.bars * info.sample_rate * 4 * 60 / std::max(bpm, (unsigned int) 1)); //FIXME Assuming its a 4/4 measure

		if (index < LOOPER_BUFFER_SIZE) {
			lout += lbuffer[index];
			rout += rbuffer[index];
		}
	}
}

void LooperChannel::record(double lin, double rin, const Metronome& metronome, const SampleInfo& info) {
	unsigned int bpm = metronome.get_bpm();
	size_t index = info.sample_time % (preset.bars * info.sample_rate * 4 * 60 / std::max(bpm, (unsigned int) 1)); //FIXME Assuming its a 4/4 measure

	if (index < LOOPER_BUFFER_SIZE) {
		//Record
		lbuffer[index] += lin;
		rbuffer[index] += rin;
	}
}

void Looper::apply_program(PluginProgram *prog) {
}

void Looper::process(const SampleInfo &info) {
	outputs[0] = inputs[0];
	outputs[1] = inputs[1];
	if (active) {
		const Metronome& metronome = get_host().get_metronome();
		if (solo_channel >= 0 && solo_channel < LOOPER_CHANNELS) {
			channels[solo_channel].apply(outputs[0], outputs[1], metronome, info);
		}
		else {
			for (size_t i = 0; i < LOOPER_CHANNELS; ++i) {
				channels[i].apply(outputs[0], outputs[1], metronome, info);
			}
		}
		if (record_channel >= 0 && record_channel < LOOPER_CHANNELS) {
			channels[record_channel].record(inputs[0], inputs[1], metronome, info);
		}
	}
}

ViewController* Looper::create_view() {
	return new EffectView(this);
}

void Looper::save_program(PluginProgram **prog) {
}

void Looper::recieve_midi(const MidiMessage &message, const SampleInfo &info) {
}

void LooperProgram::load(boost::property_tree::ptree tree) {

}

std::string LooperProgram::get_plugin_name() {
	return LOOPER_IDENTIFIER;
}

boost::property_tree::ptree LooperProgram::save() {
	return {};
}
