/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "phaser.h"
#include <cmath>
#include "../view/EffectView.h"

PhaserEffect::PhaserEffect(PluginHost& h, Plugin& p) : Effect(h, p) {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.center_cutoff);
	cc.add_binding(&preset.lfo_depth);
	cc.add_binding(&preset.lfo_rate);
	cc.add_binding(&preset.mix);

	cc.init(get_host().get_binding_handler(), this);
}

void PhaserEffect::process(const SampleInfo& info) {
	outputs[0] = inputs[0];
	outputs[1] = inputs[1];
	if (preset.on) {
		//Cutoff
		AnalogOscilatorData data = {preset.vibrato_waveform};
		osc.process(preset.lfo_rate, info.time_step, data);
		double carrier = osc.carrier(preset.lfo_rate, info.time_step, data);
		double cutoff = fmax(fmin(preset.center_cutoff + carrier * preset.lfo_depth, 1), 0);

		//Allpass
		FilterData filter{FilterType::LP_6, scale_cutoff(cutoff), 0};
		double l = inputs[0];
		double r = inputs[1];
		for (size_t i = 0; i < PHASER_ALLPASS_AMOUNT; ++i) {
			l = 2 * lfilter[i].apply(filter, l, info.time_step) - l;
			r = 2 * rfilter[i].apply(filter, r, info.time_step) - r;
		}

		//Mix
		outputs[0] *= 1 - (fmax(0, preset.mix - 0.5) * 2);
		outputs[1] *= 1 - (fmax(0, preset.mix - 0.5) * 2);

		outputs[0] += l * fmin(0.5, preset.mix) * 2;
		outputs[1] += r * fmin(0.5, preset.mix) * 2;
	}
}

ViewController* PhaserEffect::create_menu() {
	return new EffectView(this);
}

PhaserEffect::~PhaserEffect() {

}

void PhaserProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.lfo_rate.load(tree, "lfo_rate", 1);
	preset.lfo_depth.load(tree, "lfo_depth", 0.25);
	preset.mix.load(tree, "mix", 0.5);

	preset.center_cutoff.load(tree, "center_cutoff", 0.5);
	preset.vibrato_waveform = (AnalogWaveForm) tree.get<unsigned int>("vibrato_waveform", (unsigned int) AnalogWaveForm::TRIANGLE_WAVE);
}

boost::property_tree::ptree PhaserProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("lfo_rate", preset.lfo_rate.save());
	tree.add_child("lfo_depth", preset.lfo_depth.save());
	tree.add_child("mix", preset.mix.save());

	tree.add_child("center_cutoff", preset.center_cutoff.save());
	tree.put("vibrato_waveform", (unsigned int) preset.vibrato_waveform);
	return tree;
}

void PhaserEffect::save_program(PluginProgram **prog) {
	PhaserProgram* p = dynamic_cast<PhaserProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new PhaserProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void PhaserEffect::apply_program(PluginProgram *prog) {
	PhaserProgram* p = dynamic_cast<PhaserProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}

std::string PhaserProgram::get_plugin_name() {
	return PHASER_IDENTIFIER;
}
