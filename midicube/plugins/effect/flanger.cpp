/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "flanger.h"
#include <cmath>
#include "../view/EffectView.h"

FlangerEffect::FlangerEffect(PluginHost& h, Plugin& p) : Effect(h, p) {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.vibrato_rate);
	cc.add_binding(&preset.vibrato_depth);
	cc.add_binding(&preset.mix);

	cc.add_binding(&preset.delay);
	cc.add_binding(&preset.feedback);
}

void FlangerEffect::process(const SampleInfo& info) {
	outputs[0] = inputs[0];
	outputs[1] = inputs[1];
	if (preset.on) {
		double l = ldelay.process();
		double r = rdelay.process();

		//Delay
		AnalogOscilatorData data = {preset.vibrato_waveform};
		osc.process(preset.vibrato_rate, info.time_step, data);
		double del = (1 + osc.carrier(preset.vibrato_rate, info.time_step, data) * preset.vibrato_depth * 0.2) * preset.delay * info.sample_rate;
		ldelay.add_isample(outputs[0] + l * preset.feedback, del);
		rdelay.add_isample(outputs[1] + r * preset.feedback, del);

		//Mix
		outputs[0] *= 1 - (fmax(0, preset.mix - 0.5) * 2);
		outputs[1] *= 1 - (fmax(0, preset.mix - 0.5) * 2);

		outputs[0] += l * fmin(0.5, preset.mix) * 2;
		outputs[1] += r * fmin(0.5, preset.mix) * 2;
	}
}

Menu* FlangerEffect::create_menu() {
	return new FunctionMenu([this](){ return new EffectView(this); }, nullptr);
}


FlangerEffect::~FlangerEffect() {

}

void FlangerProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.vibrato_rate.load(tree, "vibrato_rate", 2);
	preset.vibrato_depth.load(tree, "vibrato_depth", 0.5);
	preset.mix.load(tree, "mix", 0.5);

	preset.delay.load(tree, "delay", 0.015);
	preset.feedback.load(tree, "feedback", 0);
	preset.vibrato_waveform = (AnalogWaveForm) tree.get<unsigned int>("vibrato_waveform", (unsigned int) AnalogWaveForm::TRIANGLE_WAVE);
}

boost::property_tree::ptree FlangerProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("vibrato_rate", preset.vibrato_rate.save());
	tree.add_child("vibrato_depth", preset.vibrato_depth.save());
	tree.add_child("mix", preset.mix.save());

	tree.add_child("delay", preset.delay.save());
	tree.add_child("feedback", preset.feedback.save());
	tree.put("vibrato_waveform", (unsigned int) preset.vibrato_waveform);
	return tree;
}

void FlangerEffect::save_program(PluginProgram **prog) {
	FlangerProgram* p = dynamic_cast<FlangerProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new FlangerProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void FlangerEffect::apply_program(PluginProgram *prog) {
	FlangerProgram* p = dynamic_cast<FlangerProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}

std::string FlangerProgram::get_plugin_name() {
	return FLANGER_IDENTIFIER;
}

