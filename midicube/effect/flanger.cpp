/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "flanger.h"
#include <cmath>

FlangerEffect::FlangerEffect() {
	cc.register_binding(new TemplateControlBinding<bool>("on", preset.on, false, true));
	cc.register_binding(new TemplateControlBinding<double>("vibrato_rate", preset.vibrato_rate, 0, 8));
	cc.register_binding(new TemplateControlBinding<double>("vibrato_depth", preset.vibrato_depth, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("mix", preset.mix, 0, 1));

	cc.register_binding(new TemplateControlBinding<double>("feedback", preset.feedback, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("delay", preset.delay, 0, 0.02));
}

void FlangerEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		double l = ldelay.process();
		double r = rdelay.process();

		//Delay
		AnalogOscilatorData data = {preset.vibrato_waveform};
		osc.process(preset.vibrato_rate, info.time_step, data);
		double del = (1 + osc.carrier(preset.vibrato_rate, info.time_step, data) * preset.vibrato_depth * 0.2) * preset.delay * info.sample_rate;
		ldelay.add_isample(lsample + l * preset.feedback, del);
		rdelay.add_isample(rsample + r * preset.feedback, del);

		//Mix
		lsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);
		rsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);

		lsample += l * fmin(0.5, preset.mix) * 2;
		rsample += r * fmin(0.5, preset.mix) * 2;
	}
}

FlangerEffect::~FlangerEffect() {

}

template<>
std::string get_effect_name<FlangerEffect>() {
	return "Flanger";
}

template <>
EffectProgram* create_effect_program<FlangerEffect>() {
	return new FlangerProgram();
}


void FlangerProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on = tree.get<bool>("on", true);
	preset.vibrato_rate = tree.get<double>("vibrato_rate", 2);
	preset.vibrato_depth = tree.get<double>("vibrato_depth", 0.5);
	preset.mix = tree.get<double>("mix", 0.5);

	preset.delay = tree.get<double>("delay", 0.015);
	preset.feedback = tree.get<double>("feedback", 0);
	preset.vibrato_waveform = (AnalogWaveForm) tree.get<unsigned int>("vibrato_waveform", (unsigned int) AnalogWaveForm::TRIANGLE_WAVE);
}

boost::property_tree::ptree FlangerProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("vibrato_rate", preset.vibrato_rate);
	tree.put("vibrato_depth", preset.vibrato_depth);
	tree.put("mix", preset.mix);

	tree.put("delay", preset.delay);
	tree.put("feedback", preset.feedback);
	tree.put("vibrato_waveform", (unsigned int) preset.vibrato_waveform);
	return tree;
}

void FlangerEffect::save_program(EffectProgram **prog) {
	FlangerProgram* p = dynamic_cast<FlangerProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new FlangerProgram();
	}
	p->ccs = cc.get_ccs();
	p->preset = preset;
	*prog = p;
}

void FlangerEffect::apply_program(EffectProgram *prog) {
	FlangerProgram* p = dynamic_cast<FlangerProgram*>(prog);
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
