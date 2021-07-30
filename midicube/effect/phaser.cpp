/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "phaser.h"
#include <cmath>

PhaserEffect::PhaserEffect() {

}

void PhaserEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		//Cutoff
		AnalogOscilatorData data = {preset.vibrato_waveform};
		osc.process(preset.lfo_rate, info.time_step, data);
		double carrier = osc.carrier(preset.lfo_rate, info.time_step, data);
		double cutoff = fmax(fmin(preset.center_cutoff + carrier * preset.lfo_depth, 1), 0);

		//Allpass
		FilterData filter{FilterType::LP_6, scale_cutoff(cutoff), 0};
		double l = lsample;
		double r = rsample;
		for (size_t i = 0; i < PHASER_ALLPASS_AMOUNT; ++i) {
			l = 2 * lfilter[i].apply(filter, l, info.time_step) - l;
			r = 2 * rfilter[i].apply(filter, r, info.time_step) - r;
		}

		//Mix
		lsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);
		rsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);

		lsample += l * fmin(0.5, preset.mix) * 2;
		rsample += r * fmin(0.5, preset.mix) * 2;
	}
}

PhaserEffect::~PhaserEffect() {

}

template<>
std::string get_effect_name<PhaserEffect>() {
	return "Phaser";
}

template <>
EffectProgram* create_effect_program<PhaserEffect>() {
	return new PhaserProgram();
}


void PhaserProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on = tree.get<bool>("on", true);
	preset.lfo_rate = tree.get<double>("lfo_rate", 1);
	preset.lfo_depth = tree.get<double>("lfo_depth", 0.25);
	preset.mix = tree.get<double>("mix", 0.5);

	preset.center_cutoff = tree.get<double>("center_cutoff", 0.5);
	preset.vibrato_waveform = (AnalogWaveForm) tree.get<unsigned int>("vibrato_waveform", (unsigned int) AnalogWaveForm::TRIANGLE_WAVE);
}

boost::property_tree::ptree PhaserProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("lfo_rate", preset.lfo_rate);
	tree.put("lfo_depth", preset.lfo_depth);
	tree.put("mix", preset.mix);

	tree.put("center_cutoff", preset.center_cutoff);
	tree.put("vibrato_waveform", (unsigned int) preset.vibrato_waveform);
	return tree;
}

void PhaserEffect::save_program(EffectProgram **prog) {
	PhaserProgram* p = dynamic_cast<PhaserProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new PhaserProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void PhaserEffect::apply_program(EffectProgram *prog) {
	PhaserProgram* p = dynamic_cast<PhaserProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}
