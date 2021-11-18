/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */


#include "bitcrusher.h"
#include <cmath>
#include "../view/EffectView.h"

BitCrusherEffect::BitCrusherEffect(PluginHost& h, Plugin& p) : Effect(h, p) {
	cc.add_binding(&preset.on);
	cc.init(get_host().get_binding_handler(), this);
}

void BitCrusherEffect::process(const SampleInfo& info) {
	if (preset.on) {
		double accuracy = pow(2, preset.bits - 1);
		outputs[0] = (inputs[0] * accuracy)/(accuracy);
		outputs[1] = (inputs[1] * accuracy)/(accuracy);
	}
	else {
		outputs[0] = inputs[0];
		outputs[1] = inputs[1];
	}
}

BitCrusherEffect::~BitCrusherEffect() {

}

void BitCrusherProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.bits = tree.get<unsigned int>("bits", 16);
}

boost::property_tree::ptree BitCrusherProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.put("bits", preset.bits);
	return tree;
}

void BitCrusherEffect::save_program(PluginProgram **prog) {
	BitCrusherProgram* p = dynamic_cast<BitCrusherProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new BitCrusherProgram();
	}
	p->preset = preset;
	*prog = p;
}

void BitCrusherEffect::apply_program(PluginProgram *prog) {
	BitCrusherProgram* p = dynamic_cast<BitCrusherProgram*>(prog);
	//Create new
	if (p) {
		preset = p->preset;
	}
	else {
		preset = {};
	}
}

ViewController* BitCrusherEffect::create_view() {
	return new EffectView(this);
}


std::string BitCrusherProgram::get_plugin_name() {
	return BIT_CRUSHER_IDENTIFIER;
}
