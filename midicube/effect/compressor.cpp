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
		double l = ldelay.process();
		double r = rdelay.process();

		//Delay
		AnalogOscilatorData data = {preset.vibrato_waveform};
		osc.process(preset.vibrato_rate, info.time_step, data);
		double del = (1 + osc.carrier(preset.vibrato_rate, info.time_step, data) * preset.vibrato_depth * 0.2) * preset.delay * info.sample_rate;
		ldelay.add_isample(lsample, del);
		rdelay.add_isample(rsample, del);

		//Mix
		lsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);
		rsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);

		lsample += l * fmin(0.5, preset.mix) * 2;
		rsample += r * fmin(0.5, preset.mix) * 2;
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
