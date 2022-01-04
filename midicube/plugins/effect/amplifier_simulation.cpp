/*
 * amplifier_simulation.cpp
 *
 *  Created on: Jan 7, 2021
 *      Author: jojo
 */

#include "amplifier_simulation.h"
#include "../view/EffectView.h"

AmplifierSimulationEffect::AmplifierSimulationEffect(PluginHost& h, Plugin& plugin) : Effect(h, plugin) {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.post_gain);
	cc.add_binding(&preset.drive);
	cc.add_binding(&preset.tone);
	cc.add_binding(&preset.high_freq);
	cc.add_binding(&preset.high_gain);
	cc.add_binding(&preset.mid_freq);
	cc.add_binding(&preset.mid_gain);
	cc.add_binding(&preset.low_freq);
	cc.add_binding(&preset.low_gain);
	cc.add_binding(&preset.lowshelf_boost);
	cc.add_binding(&preset.lowshelf_cutoff);
}

void AmplifierSimulationEffect::process(const SampleInfo &info) {
	double lsample = inputs[0];
	double rsample = inputs[1];
	if (preset.on) {
		AmplifierSimulationData<AMPLIFIER_SIMULATION_EQ_BANDS> data;
		data.post_gain = preset.post_gain;
		data.tone = preset.tone;
		data.triode = {preset.drive, preset.type, 1, preset.lowshelf_cutoff, preset.lowshelf_boost};
		data.eq.bands = {EqualizerBand{preset.low_freq, preset.low_gain, FilterType::LP_6}, EqualizerBand{preset.mid_freq, preset.mid_gain, FilterType::BP_12}, EqualizerBand{preset.high_freq, preset.high_gain, FilterType::HP_6}};
		lsample = lamp.apply(lsample, data, info.time_step);
		rsample = ramp.apply(rsample, data, info.time_step);
	}
	outputs[0] = lsample;
	outputs[1] = rsample;
}

Menu* AmplifierSimulationEffect::create_menu() {
	return new FunctionMenu([this](){ return new EffectView(this); }, nullptr);
}

AmplifierSimulationEffect::~AmplifierSimulationEffect() {

}

void AmplifierSimulationProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.post_gain.load(tree, "post_gain", 0);
	preset.drive.load(tree, "drive", 0);
	preset.tone.load(tree, "tone", 0.6);
	preset.low_freq.load(tree, "low_freq", 100);
	preset.low_gain.load(tree, "low_gain", 0);
	preset.mid_freq.load(tree, "mid_freq", 1000);
	preset.mid_gain.load(tree, "mid_gain", 0);
	preset.high_freq.load(tree, "high_freq", 4000);
	preset.high_gain.load(tree, "high_gain", 0);
	preset.lowshelf_boost.load(tree, "lowshelf_boost", 2);
	preset.lowshelf_cutoff.load(tree, "lowshelf_cutoff", 80);
}

boost::property_tree::ptree AmplifierSimulationProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("post_gain", preset.post_gain.save());
	tree.add_child("drive", preset.drive.save());
	tree.add_child("tone", preset.tone.save());
	tree.add_child("low_freq", preset.low_freq.save());
	tree.add_child("low_gain", preset.low_gain.save());
	tree.add_child("mid_freq", preset.mid_freq.save());
	tree.add_child("mid_gain", preset.mid_gain.save());
	tree.add_child("high_freq", preset.high_freq.save());
	tree.add_child("high_gain", preset.high_gain.save());
	tree.add_child("lowshelf_boost", preset.lowshelf_boost.save());
	tree.add_child("lowshelf_cutoff", preset.lowshelf_cutoff.save());
	return tree;
}


void AmplifierSimulationEffect::save_program(PluginProgram **prog) {
	AmplifierSimulationProgram* p = dynamic_cast<AmplifierSimulationProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new AmplifierSimulationProgram();
	}
	p->preset = preset;
	*prog = p;
}

void AmplifierSimulationEffect::apply_program(PluginProgram *prog) {
	AmplifierSimulationProgram* p = dynamic_cast<AmplifierSimulationProgram*>(prog);
	//Create new
	if (p) {
		preset = p->preset;
	}
	else {
		preset = {};
	}
}

std::string AmplifierSimulationProgram::get_plugin_name() {
	return AMPLIFIER_SIMULATION_IDENTIFIER;
}

