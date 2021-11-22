/*
 * Equalizer.cpp
 *
 *  Created on: May 27, 2021
 *      Author: jojo
 */

#include "equalizer.h"
#include <cmath>
#include "../view/EffectView.h"

EqualizerEffect::EqualizerEffect(PluginHost& h, Plugin& p) : Effect(h, p) {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.low_freq);
	cc.add_binding(&preset.low_gain);
	cc.add_binding(&preset.low_mid_freq);
	cc.add_binding(&preset.low_mid_gain);
	cc.add_binding(&preset.mid_freq);
	cc.add_binding(&preset.mid_gain);
	cc.add_binding(&preset.high_freq);
	cc.add_binding(&preset.high_gain);

	cc.init(get_host().get_binding_handler(), this);
}

void EqualizerEffect::process(const SampleInfo& info) {
	outputs[0] = inputs[0];
	outputs[1] = inputs[1];
	if (preset.on) {
		//Filters
		NBandEqualizerData<EQ_4_BANDS> bands = {{
				EqualizerBand{preset.low_freq, preset.low_gain, FilterType::LP_6},
				EqualizerBand{preset.low_mid_freq, preset.low_mid_gain, FilterType::BP_12},
				EqualizerBand{preset.mid_freq, preset.mid_gain, FilterType::BP_12},
				EqualizerBand{preset.high_freq, preset.high_gain, FilterType::HP_6}
			}};

		outputs[0] = leq.apply(outputs[0], bands, info.time_step);
		outputs[1] = req.apply(outputs[1], bands, info.time_step);
	}
}

ViewController* EqualizerEffect::create_view() {
	return new EffectView(this);
}


EqualizerEffect::~EqualizerEffect() {

}

void EqualizerProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.low_freq.load(tree, "low_freq", 100);
	preset.low_gain.load(tree, "low_gain", 0);
	preset.low_mid_freq.load(tree, "low_mid_freq", 400);
	preset.low_mid_gain.load(tree, "low_mid_gain", 0);
	preset.mid_freq.load(tree, "mid_freq", 1000);
	preset.mid_gain.load(tree, "mid_gain", 0);
	preset.high_freq.load(tree, "high_freq", 400);
	preset.high_gain.load(tree, "high_gain", 0);
}

boost::property_tree::ptree EqualizerProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("low_freq", preset.low_freq.save());
	tree.add_child("low_gain", preset.low_gain.save());
	tree.add_child("low_mid_freq", preset.low_mid_freq.save());
	tree.add_child("low_mid_gain", preset.low_mid_gain.save());
	tree.add_child("mid_freq", preset.mid_freq.save());
	tree.add_child("mid_gain", preset.mid_gain.save());
	tree.add_child("high_freq", preset.high_freq.save());
	tree.add_child("high_gain", preset.high_gain.save());
	return tree;
}

void EqualizerEffect::save_program(PluginProgram **prog) {
	EqualizerProgram* p = dynamic_cast<EqualizerProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new EqualizerProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void EqualizerEffect::apply_program(PluginProgram *prog) {
	EqualizerProgram* p = dynamic_cast<EqualizerProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}

std::string EqualizerProgram::get_plugin_name() {
	return EQUALIZER_IDENTIFIER;
}
