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
	cc.init(get_host().get_binding_handler(), this);
}

void AmplifierSimulationEffect::process(const SampleInfo &info) {
	double lsample = inputs[0];
	double rsample = inputs[1];
	if (preset.on) {
		AmplifierSimulationData data = {preset.drive, preset.post_gain, preset.tone, preset.type};
		lsample = lamp.apply(lsample, data, info.time_step);
		rsample = ramp.apply(rsample, data, info.time_step);
	}
	outputs[0] = lsample;
	outputs[1] = rsample;
}

ViewController* AmplifierSimulationEffect::create_view() {
	return new EffectView(this);
}

AmplifierSimulationEffect::~AmplifierSimulationEffect() {

}

void AmplifierSimulationProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.post_gain.load(tree, "post_gain", 0);
	preset.drive.load(tree, "drive", 0);
	preset.tone.load(tree, "tone", 0.6);
}

boost::property_tree::ptree AmplifierSimulationProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("post_gain", preset.post_gain.save());
	tree.add_child("drive", preset.drive.save());
	tree.add_child("tone", preset.tone.save());
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
