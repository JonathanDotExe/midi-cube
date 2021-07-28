/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "compressor.h"
#include <cmath>

CompressorEffect::CompressorEffect() {

}

void CompressorEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		//Get volume
		lenv.apply(lsample, info.time_step);
		renv.apply(rsample, info.time_step);
		double lv = lenv.volume();
		double rv = renv.volume();

		double lcomp = 1;
		double rcomp = 1;

		double threshold = db_to_amp(preset.threshold);

		//Compress
		double lslope = preset.release;
		if (lv > threshold) {
			lslope = preset.attack;
			lcomp = pow(lv/threshold, 1/preset.ratio) * threshold / lv;
		}
		double rslope = preset.release;
		if (rv > threshold) {
			rslope = preset.attack;
			rcomp = pow(rv/threshold, 1/preset.ratio) * threshold / rv;
		}

		//Volume
		lvol.set(lcomp, info.time, lslope);
		rvol.set(rcomp, info.time, rslope);

		//Apply
		lsample *= lvol.get(info.time) * preset.makeup_gain;
		rsample *= rvol.get(info.time) * preset.makeup_gain;
	}
}

CompressorEffect::~CompressorEffect() {

}

template<>
std::string get_effect_name<CompressorEffect>() {
	return "Compressor";
}

template <>
EffectProgram* create_effect_program<CompressorEffect>() {
	return new CompressorProgram();
}


void CompressorProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on = tree.get<bool>("on", true);
	preset.threshold = tree.get<double>("threshold", -20);
	preset.ratio = tree.get<double>("ratio", 4);
	preset.attack = tree.get<double>("attack", 0.1);
	preset.release = tree.get<double>("release", 0.1);
	preset.makeup_gain = tree.get<double>("makeup_gain", 1);

}

boost::property_tree::ptree CompressorProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("threshold", preset.threshold);
	tree.put("ratio", preset.ratio);
	tree.put("attack", preset.attack);
	tree.put("release", preset.release);
	tree.put("makeup_gain", preset.makeup_gain);
	return tree;
}

void CompressorEffect::save_program(EffectProgram **prog) {
	CompressorProgram* p = dynamic_cast<CompressorProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new CompressorProgram();
	}
	p->ccs = cc.get_ccs();
	p->preset = preset;
	*prog = p;
}

void CompressorEffect::apply_program(EffectProgram *prog) {
	CompressorProgram* p = dynamic_cast<CompressorProgram*>(prog);
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
