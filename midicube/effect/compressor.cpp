/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "compressor.h"
#include <cmath>

CompressorEffect::CompressorEffect() {
	cc.register_binding(new TemplateControlBinding<bool>("on", preset.on, false, true));
	cc.register_binding(new TemplateControlBinding<double>("treshold", preset.treshold, 0, 2));
	cc.register_binding(new TemplateControlBinding<double>("ratio", preset.ratio, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("attack", preset.attack, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("release", preset.release, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("makeup_gain", preset.makeup_gain, 0, 5));
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

		//Compress
		double lslope = preset.release;
		if (lv > preset.treshold) {
			lslope = preset.attack;
			lcomp = pow(lv/preset.treshold, 1/preset.ratio) * preset.treshold / lv;
		}
		double rslope = preset.release;
		if (rv > preset.treshold) {
			rslope = preset.attack;
			rcomp = pow(rv/preset.treshold, 1/preset.ratio) * preset.treshold / rv;
		}

		//Volume
		lvol.set(lcomp, info.time, lslope);
		rvol.set(rcomp, info.time, rslope);

		//Apply
		lsample *= lvol.get(info.time);
		rsample *= rvol.get(info.time);
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
	preset.treshold = tree.get<double>("treshold", 0.2);
	preset.ratio = tree.get<double>("ratio", 4);
	preset.attack = tree.get<double>("attack", 0.1);
	preset.release = tree.get<double>("release", 0.1);
	preset.makeup_gain = tree.get<double>("makeup_gain", 0);

}

boost::property_tree::ptree CompressorProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("treshold", preset.treshold);
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
