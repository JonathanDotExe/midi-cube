/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "tremolo.h"
#include <cmath>

TremoloEffect::TremoloEffect() {

}

void TremoloEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		//Tremolo
		AnalogOscilatorData data = {preset.waveform};
		osc.process(preset.rate, info.time_step, data);
		double mul = (1 - preset.depth) + preset.depth * (osc.carrier(preset.rate, info.time_step, data) + 1)/2.0;

		//Apply
		lsample *= mul;
		rsample *= mul;
	}
}

TremoloEffect::~TremoloEffect() {

}

template<>
std::string get_effect_name<TremoloEffect>() {
	return "Tremolo";
}

template <>
EffectProgram* create_effect_program<TremoloEffect>() {
	return new TremoloProgram();
}


void TremoloProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on = tree.get<bool>("on", true);
	preset.rate = tree.get<double>("rate", 2);
	preset.depth = tree.get<double>("depth", 0.5);
	preset.waveform = (AnalogWaveForm) tree.get<unsigned int>("waveform", (unsigned int) AnalogWaveForm::TRIANGLE_WAVE);
}

boost::property_tree::ptree TremoloProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("rate", preset.rate);
	tree.put("depth", preset.depth);
	tree.put("waveform", (unsigned int) preset.waveform);
	return tree;
}

void TremoloEffect::save_program(EffectProgram **prog) {
	TremoloProgram* p = dynamic_cast<TremoloProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new TremoloProgram();
	}
	p->ccs = cc.get_ccs();
	p->preset = preset;
	*prog = p;
}

void TremoloEffect::apply_program(EffectProgram *prog) {
	TremoloProgram* p = dynamic_cast<TremoloProgram*>(prog);
	//Create new
	if (p) {
		cc.set_ccs(p->ccs);
		preset = p->preset;
	}
	else {
		cc.set_ccs({});
		preset = {};
	}
}
