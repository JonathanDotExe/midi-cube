/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "compressor.h"
#include <cmath>
#include "../view/EffectView.h"

CompressorEffect::CompressorEffect(PluginHost& h, Plugin& p) : Effect(h, p) {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.threshold);
	cc.add_binding(&preset.ratio);
	cc.add_binding(&preset.attack);
	cc.add_binding(&preset.release);
	cc.add_binding(&preset.makeup_gain);

	cc.init(get_host().get_binding_handler(), this);
}

void CompressorEffect::process(const SampleInfo& info) {
	outputs[0] = inputs[0];
	outputs[1] = inputs[1];
	if (preset.on) {
		//Get volume
		lenv.apply(inputs[0], info.time_step);
		renv.apply(inputs[1], info.time_step);
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
		lvol.set(lcomp, info.time, lslope, 1);
		rvol.set(rcomp, info.time, rslope, 1);

		//Apply
		outputs[0] *= lvol.get(info.time) * preset.makeup_gain;
		outputs[1] *= rvol.get(info.time) * preset.makeup_gain;
	}
}

CompressorEffect::~CompressorEffect() {

}

void CompressorProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.threshold.load(tree, "threshold", -20);
	preset.ratio.load(tree, "ratio", 4);
	preset.attack.load(tree, "attack", 0.1);
	preset.release.load(tree, "release", 0.1);
	preset.makeup_gain.load(tree, "makeup_gain", 1);

}

boost::property_tree::ptree CompressorProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("threshold", preset.threshold.save());
	tree.add_child("ratio", preset.ratio.save());
	tree.add_child("attack", preset.attack.save());
	tree.add_child("release", preset.release.save());
	tree.add_child("makeup_gain", preset.makeup_gain.save());
	return tree;
}

void CompressorEffect::save_program(PluginProgram **prog) {
	CompressorProgram* p = dynamic_cast<CompressorProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new CompressorProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void CompressorEffect::apply_program(PluginProgram *prog) {
	CompressorProgram* p = dynamic_cast<CompressorProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}

ViewController* CompressorEffect::create_view() {
	return new EffectView(this);
}


std::string CompressorProgram::get_plugin_name() {
	return COMPRESSOR_IDENTIFIER;
}
