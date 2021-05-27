/*
 * Equalizer.cpp
 *
 *  Created on: May 27, 2021
 *      Author: jojo
 */

#include "equalizer.h"
#include <cmath>

EqualizerEffect::EqualizerEffect() {
	cc.register_binding(new TemplateControlBinding<bool>("on", preset.on, false, true));
	cc.register_binding(new TemplateControlBinding<double>("low_freq", preset.low_freq, 20, 400));
	cc.register_binding(new TemplateControlBinding<double>("low_gain", preset.low_gain, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("low_mid_freq", preset.low_mid_freq, 100, 1000));
	cc.register_binding(new TemplateControlBinding<double>("low_mid_gain", preset.low_mid_gain, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("mid_freq", preset.mid_freq, 200, 8000));
	cc.register_binding(new TemplateControlBinding<double>("mid_gain", preset.mid_gain, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("high_freq", preset.high_freq, 1000, 20000));
	cc.register_binding(new TemplateControlBinding<double>("high_gain", preset.high_gain, 0, 1));
}

void EqualizerEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
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
	preset.on = tree.get<bool>("on", true);
	preset.low_freq = tree.get<double>("low_freq", 100);
	preset.low_gain = tree.get<double>("low_gain", 0);
	preset.low_mid_freq = tree.get<double>("low_mid_freq", 400);
	preset.low_mid_gain = tree.get<double>("low_mid_gain", 0);
	preset.mid_freq = tree.get<double>("mid_freq", 1000);
	preset.mid_gain = tree.get<double>("mid_gain", 0);
	preset.high_freq = tree.get<double>("high_freq", 400);
	preset.high_gain = tree.get<double>("high_gain", 0);
}

boost::property_tree::ptree EqualizerProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("low_freq", preset.low_freq);
	tree.put("low_gain", preset.low_gain);
	tree.put("low_mid_freq", preset.low_mid_freq);
	tree.put("low_mid_gain", preset.low_mid_gain);
	tree.put("mid_freq", preset.mid_freq);
	tree.put("mid_gain", preset.mid_gain);
	tree.put("high_freq", preset.high_freq);
	tree.put("high_gain", preset.high_gain);
	return tree;
}

void EqualizerEffect::save_program(EffectProgram **prog) {
	EqualizerProgram* p = dynamic_cast<EqualizerProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new EqualizerProgram();
	}
	p->ccs = cc.get_ccs();
	p->preset = preset;
	*prog = p;
}

void EqualizerEffect::apply_program(EffectProgram *prog) {
	EqualizerProgram* p = dynamic_cast<EqualizerProgram*>(prog);
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
