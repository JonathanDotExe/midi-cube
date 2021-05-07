/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "delay.h"
#include <cmath>

DelayEffect::DelayEffect() {
	cc.register_binding(new TemplateControlBinding<bool>("on", preset.on, false, true));
	cc.register_binding(new TemplateControlBinding<double>("mix", preset.mix, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("delay", preset.delay, 0, 5));
	cc.register_binding(new TemplateControlBinding<double>("feedback", preset.feedback, 0, 1));
}

void DelayEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		double l = ldelay.process();
		double r = rdelay.process();

		//Delay
		ldelay.add_sample(lsample + l * preset.feedback, preset.delay * info.sample_rate);
		rdelay.add_sample(rsample + r * preset.feedback, preset.delay * info.sample_rate);

		//Mix
		lsample = lsample * (1 - preset.mix) + l * preset.mix;
		rsample = rsample * (1 - preset.mix) + r * preset.mix;
	}
}

DelayEffect::~DelayEffect() {

}

template<>
std::string get_effect_name<DelayEffect>() {
	return "Delay";
}

template <>
EffectProgram* create_effect_program<DelayEffect>() {
	return new DelayProgram();
}


void DelayProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on = tree.get<bool>("on", true);
	preset.mix = tree.get<double>("mix", 0.);
	preset.delay = tree.get<double>("delay", 0.1);
	preset.feedback = tree.get<double>("feedback", 0.2);
}

boost::property_tree::ptree DelayProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("mix", preset.mix);
	tree.put("delay", preset.delay);
	tree.put("feedback", preset.feedback);
	return tree;
}

void DelayEffect::save_program(EffectProgram **prog) {
	DelayProgram* p = dynamic_cast<DelayProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new DelayProgram();
	}
	p->ccs = cc.get_ccs();
	p->preset = preset;
	*prog = p;
}

void DelayEffect::apply_program(EffectProgram *prog) {
	DelayProgram* p = dynamic_cast<DelayProgram*>(prog);
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
