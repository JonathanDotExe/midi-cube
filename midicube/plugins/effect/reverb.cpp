/*
 * reverb.cpp
 *
 *  Created on: 20 Apr 2021
 *      Author: jojo
 */

#include "reverb.h"

#include <cmath>
#include "../view/EffectView.h"

ReverbEffect::ReverbEffect(PluginHost& h, Plugin& p) : Effect(h, p) {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.delay);
	cc.add_binding(&preset.decay);
	cc.add_binding(&preset.mix);
	cc.add_binding(&preset.tone);
	cc.add_binding(&preset.resonance);
	cc.add_binding(&preset.stereo);

	cc.init(get_host().get_binding_handler(), this);
}

void ReverbEffect::process(const SampleInfo &info) {
	outputs[0] = inputs[0];
	outputs[1] = inputs[1];
	if (preset.on) {
		double l = 0;
		double r = 0;
		//Reverb
		SchroederReverbData data;
		data.delay = preset.delay;
		data.feedback = preset.decay;
		l = lreverb.apply(outputs[0], data, info);
		data.delay *= 1 + (preset.stereo * 0.2);
		data.feedback *= 1 + (preset.stereo * 0.1); //TODO better stereo
		r = rreverb.apply(outputs[1], data, info);

		//Lowpass
		FilterData d = {FilterType::LP_12, scale_cutoff(preset.tone), preset.resonance};
		l = lfilter.apply(d, l, info.time_step);
		r = lfilter.apply(d, r, info.time_step);
		//Mix
		mix(outputs[0], l ,preset.mix);
		mix(outputs[1], r ,preset.mix);
	}
}

void ReverbEffect::save_program(PluginProgram **prog) {
	ReverbProgram* p = dynamic_cast<ReverbProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new ReverbProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void ReverbEffect::apply_program(PluginProgram *prog) {
	ReverbProgram* p = dynamic_cast<ReverbProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}

Menu* ReverbEffect::create_menu() {
	return new FunctionMenu([this](){ return new EffectView(this); }, nullptr);
}

ReverbEffect::~ReverbEffect() {
}


void ReverbProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.delay.load(tree, "delay", 0.2);
	preset.decay.load(tree, "decay", 0.7);
	preset.mix.load(tree, "mix", 0.5);

	preset.tone.load(tree, "tone", 0.35);
	preset.resonance.load(tree, "resonance", 0.0);
	preset.stereo.load(tree, "stereo", 0.0);
}

boost::property_tree::ptree ReverbProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("delay", preset.delay.save());
	tree.add_child("decay", preset.decay.save());
	tree.add_child("mix", preset.mix.save());

	tree.add_child("tone", preset.tone.save());
	tree.add_child("resonance", preset.resonance.save());
	tree.add_child("stereo", preset.stereo.save());
	return tree;
}

std::string ReverbProgram::get_plugin_name() {
	return REVERB_IDENTIFIER;
}
