/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "chorus.h"
#include <cmath>

ChorusEffect::ChorusEffect() {
	cc.register_binding(new TemplateControlBinding<bool>("on", preset.on, false, true));
	cc.register_binding(new TemplateControlBinding<double>("vibrato_rate", preset.vibrato_rate, 0, 8));
	cc.register_binding(new TemplateControlBinding<double>("vibrato_depth", preset.vibrato_depth, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("mix", preset.mix, 0, 1));

	cc.register_binding(new TemplateControlBinding<double>("delay", preset.delay, 0, 0.03));
}

void ChorusEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		double l = ldelay.process();
		double r = rdelay.process();

		//Delay
		AnalogOscilatorData data = {preset.vibrato_waveform};
		osc.process(preset.vibrato_rate, info.time_step, data);
		double del = (1 + osc.carrier(preset.vibrato_rate, info.time_step, data) * preset.vibrato_depth) * preset.delay * 0.2 * info.sample_rate;
		ldelay.add_isample(lsample, del);
		rdelay.add_isample(rsample, del);

		//Mix
		lsample = lsample * (1 - preset.mix) + l * preset.mix;
		rsample = rsample * (1 - preset.mix) + r * preset.mix;
	}
}

ChorusEffect::~ChorusEffect() {

}

template<>
std::string get_effect_name<ChorusEffect>() {
	return "Chorus";
}

template <>
EffectProgram* create_effect_program<ChorusEffect>() {
	return new ChorusProgram();
}


void ChorusProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on = tree.get<bool>("on", true);
	preset.vibrato_rate = tree.get<double>("vibrato_rate", 2);
	preset.vibrato_depth = tree.get<double>("vibrato_depth", 0.5);
	preset.mix = tree.get<double>("mix", 0.5);

	preset.delay = tree.get<double>("delay", 0.015);
	preset.vibrato_waveform = (AnalogWaveForm) tree.get<unsigned int>("vibrato_waveform", (unsigned int) AnalogWaveForm::TRIANGLE_WAVE);
}

boost::property_tree::ptree ChorusProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("vibrato_rate", preset.vibrato_rate);
	tree.put("vibrato_depth", preset.vibrato_depth);
	tree.put("mix", preset.mix);

	tree.put("delay", preset.delay);
	tree.put("vibrato_waveform", (unsigned int) preset.vibrato_waveform);
	return tree;
}

void ChorusEffect::save_program(EffectProgram **prog) {
	ChorusProgram* p = dynamic_cast<ChorusProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new ChorusProgram();
	}
	p->ccs = cc.get_ccs();
	p->preset = preset;
	*prog = p;
}

void ChorusEffect::apply_program(EffectProgram *prog) {
	ChorusProgram* p = dynamic_cast<ChorusProgram*>(prog);
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
