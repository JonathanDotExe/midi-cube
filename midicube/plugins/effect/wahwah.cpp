/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "wahwah.h"
#include <cmath>
#include "../view/EffectView.h"

WahWahEffect::WahWahEffect(PluginHost& h, Plugin& p) : Effect(h, p) {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.cutoff);
	cc.add_binding(&preset.resonance);
	cc.add_binding(&preset.amount);
	cc.add_binding(&preset.mix);
	cc.add_binding(&preset.auto_wah);

	cc.init(get_host().get_binding_handler(), this);
}

void WahWahEffect::process(const SampleInfo &info) {
	outputs[0] = inputs[0];
	outputs[1] = inputs[1];
	if (preset.on) {
		//Cutoff
		FilterData ldata;
		FilterData rdata;

		double amount = fmin(1 - preset.cutoff, preset.amount);
		if (preset.auto_wah) {
			lenv.apply(outputs[0], info.time_step);
			renv.apply(outputs[1], info.time_step);

			ldata = {FilterType::LP_6, scale_cutoff(preset.cutoff + lenv.volume() * amount), preset.resonance};
			rdata = {FilterType::LP_6, scale_cutoff(preset.cutoff + renv.volume() * amount), preset.resonance};
		}
		else {
			ldata = {FilterType::LP_6, scale_cutoff(preset.cutoff + amount), preset.resonance};
			rdata = ldata;
		}

		//Filter
		double l = lfilter.apply(ldata, outputs[0], info.time_step);
		double r = rfilter.apply(rdata, outputs[1], info.time_step);

		//Mix
		outputs[0] *= 1 - (fmax(0, preset.mix - 0.5) * 2);
		outputs[1] *= 1 - (fmax(0, preset.mix - 0.5) * 2);

		outputs[0] += l * fmin(0.5, preset.mix) * 2;
		outputs[1] += r * fmin(0.5, preset.mix) * 2;
	}
}

Menu* WahWahEffect::create_menu() {
	return new FunctionMenu([this](){ return new EffectView(this); }, nullptr);
}

WahWahEffect::~WahWahEffect() {

}

void WahWahProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.auto_wah.load(tree, "auto_wah", true);
	preset.mix.load(tree, "mix", 1);
	preset.cutoff.load(tree, "cutoff", 0.2);
	preset.resonance.load(tree, "resonance", 0.5);
	preset.amount.load(tree, "amount", 0.5);
}

boost::property_tree::ptree WahWahProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("auto_wah", preset.auto_wah.save());
	tree.add_child("mix", preset.mix.save());

	tree.add_child("cutoff", preset.cutoff.save());
	tree.add_child("resonance", preset.resonance.save());
	tree.add_child("amount", preset.amount.save());
	return tree;
}

void WahWahEffect::save_program(PluginProgram **prog) {
	WahWahProgram *p = dynamic_cast<WahWahProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new WahWahProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void WahWahEffect::apply_program(PluginProgram *prog) {
	WahWahProgram *p = dynamic_cast<WahWahProgram*>(prog);
	//Create new
	if (p) {
		preset = p->preset;
	} else {
		preset = { };
	}
}

std::string WahWahProgram::get_plugin_name() {
	return WAH_WAH_IDENTIFIER;
}
