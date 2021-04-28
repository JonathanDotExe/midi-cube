/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "bitcrusher.h"
#include <cmath>

BitCrusherEffect::BitCrusherEffect() {
	cc.register_binding(new TemplateControlBinding<bool>("on", preset.on, false, true));
}

void BitCrusherEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		double accuracy = pow(2, preset.bits - 1);
		lsample = (int) (lsample * accuracy)/(accuracy);
		rsample = (int) (rsample * accuracy)/(accuracy);
	}
}

BitCrusherEffect::~BitCrusherEffect() {

}

template<>
std::string get_effect_name<BitCrusherEffect>() {
	return "Bit Crusher";
}

void BitCrusherProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on = tree.get<bool>("on", true);
	preset.bits = tree.get<unsigned int>("bits", 16);
}

boost::property_tree::ptree BitCrusherProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("bits", preset.bits);
	return tree;
}


template <>
EffectProgram* create_effect_program<BitCrusherEffect>() {
	return new BitCrusherProgram();
}


void BitCrusherEffect::save_program(EffectProgram **prog) {
	BitCrusherProgram* p = dynamic_cast<BitCrusherProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new BitCrusherProgram();
	}
	p->ccs = cc.get_ccs();
	p->preset = preset;
	*prog = p;
}

void BitCrusherEffect::apply_program(EffectProgram *prog) {
	BitCrusherProgram* p = dynamic_cast<BitCrusherProgram*>(prog);
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
