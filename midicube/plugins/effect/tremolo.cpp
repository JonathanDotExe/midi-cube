/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "tremolo.h"
#include <cmath>
#include "../view/EffectView.h"

TremoloEffect::TremoloEffect(PluginHost& h, Plugin& p) : Effect(h, p) {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.rate);
	cc.add_binding(&preset.depth);
}

void TremoloEffect::process(const SampleInfo& info) {
	outputs[0] = inputs[0];
	outputs[1] = inputs[1];
	if (preset.on) {
		//Tremolo
		AnalogOscilatorData data = {preset.waveform};
		osc.process(preset.rate, info.time_step, data);
		double mul = (1 - preset.depth) + preset.depth * (osc.carrier(preset.rate, info.time_step, data) + 1)/2.0;

		//Apply
		outputs[0] *= mul;
		outputs[1] *= mul;
	}
}

ViewController* TremoloEffect::create_view() {
	return new EffectView(this);
}

TremoloEffect::~TremoloEffect() {

}


void TremoloProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.rate.load(tree, "rate", 2);
	preset.depth.load(tree, "depth", 0.5);
	preset.waveform = (AnalogWaveForm) tree.get<unsigned int>("waveform", (unsigned int) AnalogWaveForm::TRIANGLE_WAVE);
}

boost::property_tree::ptree TremoloProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("rate", preset.rate.save());
	tree.add_child("depth", preset.depth.save());
	tree.put("waveform", (unsigned int) preset.waveform);
	return tree;
}

void TremoloEffect::save_program(PluginProgram **prog) {
	TremoloProgram* p = dynamic_cast<TremoloProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new TremoloProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void TremoloEffect::apply_program(PluginProgram *prog) {
	TremoloProgram* p = dynamic_cast<TremoloProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}

std::string TremoloProgram::get_plugin_name() {
	return TREMOLO_IDENTIFIER;
}
