/*
 * Equalizer.cpp
 *
 *  Created on: May 27, 2021
 *      Author: jojo
 */

#include "equalizer.h"
#include <cmath>

EqualizerEffect::EqualizerEffect() {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.low_freq);
	cc.add_binding(&preset.low_gain);
	cc.add_binding(&preset.low_mid_freq);
	cc.add_binding(&preset.low_mid_gain);
	cc.add_binding(&preset.mid_freq);
	cc.add_binding(&preset.mid_gain);
	cc.add_binding(&preset.high_freq);
	cc.add_binding(&preset.high_gain);
}

void EqualizerEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		//Filters
		FilterData lowdata = {FilterType::LP_6, preset.low_freq};
		FilterData low_middata = {FilterType::BP_12, preset.low_mid_freq};
		FilterData middata = {FilterType::BP_12, preset.mid_freq};
		FilterData highdata = {FilterType::HP_6, preset.high_freq};

		double llow = llowfilter.apply(lowdata, lsample, info.time_step);
		double rlow = rlowfilter.apply(lowdata, rsample, info.time_step);
		double llow_mid = llow_midfilter.apply(low_middata, lsample, info.time_step);
		double rlow_mid = rlow_midfilter.apply(low_middata, rsample, info.time_step);
		double lmid = lmidfilter.apply(middata, lsample, info.time_step);
		double rmid = rmidfilter.apply(middata, rsample, info.time_step);
		double lhigh = lhighfilter.apply(highdata, lsample, info.time_step);
		double rhigh = rhighfilter.apply(highdata, rsample, info.time_step);

		//Apply
		lsample += llow * preset.low_gain + llow_mid * preset.low_mid_gain + lmid * preset.mid_gain + lhigh * preset.high_gain;
		rsample += rlow * preset.low_gain + rlow_mid * preset.low_mid_gain + rmid * preset.mid_gain + rhigh * preset.high_gain;
	}
}

EqualizerEffect::~EqualizerEffect() {

}

template<>
std::string get_effect_name<EqualizerEffect>() {
	return "Equalizer";
}

template <>
EffectProgram* create_effect_program<EqualizerEffect>() {
	return new EqualizerProgram();
}


void EqualizerProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on.load(tree, "on", true);
	preset.low_freq.load(tree, "low_freq", 100);
	preset.low_gain.load(tree, "low_gain", 0);
	preset.low_mid_freq.load(tree, "low_mid_freq", 400);
	preset.low_mid_gain.load(tree, "low_mid_gain", 0);
	preset.mid_freq.load(tree, "mid_freq", 1000);
	preset.mid_gain.load(tree, "mid_gain", 0);
	preset.high_freq.load(tree, "high_freq", 400);
	preset.high_gain.load(tree, "high_gain", 0);
}

boost::property_tree::ptree EqualizerProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.add_child("on", preset.on.save());
	tree.add_child("low_freq", preset.low_freq.save());
	tree.add_child("low_gain", preset.low_gain.save());
	tree.add_child("low_mid_freq", preset.low_mid_freq.save());
	tree.add_child("low_mid_gain", preset.low_mid_gain.save());
	tree.add_child("mid_freq", preset.mid_freq.save());
	tree.add_child("mid_gain", preset.mid_gain.save());
	tree.add_child("high_freq", preset.high_freq.save());
	tree.add_child("high_gain", preset.high_gain.save());
	return tree;
}

void EqualizerEffect::save_program(EffectProgram **prog) {
	EqualizerProgram* p = dynamic_cast<EqualizerProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new EqualizerProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void EqualizerEffect::apply_program(EffectProgram *prog) {
	EqualizerProgram* p = dynamic_cast<EqualizerProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}
