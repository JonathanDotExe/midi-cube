/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "wahwah.h"
#include <cmath>

WahWahEffect::WahWahEffect() {

}

void WahWahEffect::apply(double &lsample, double &rsample, SampleInfo &info) {
	if (preset.on) {
		//Cutoff
		FilterData ldata;
		FilterData rdata;

		double amount = fmin(1 - preset.cutoff, preset.amount);
		if (preset.auto_wah) {
			lenv.apply(lsample, info.time_step);
			renv.apply(rsample, info.time_step);

			ldata = {FilterType::LP_6, scale_cutoff(preset.cutoff + lenv.volume() * amount), preset.resonance};
			rdata = {FilterType::LP_6, scale_cutoff(preset.cutoff + renv.volume() * amount), preset.resonance};
		}
		else {
			ldata = {FilterType::LP_6, scale_cutoff(preset.cutoff + pedal * amount), preset.resonance};
			rdata = ldata;
		}

		//Filter
		double l = lfilter.apply(ldata, lsample, info.time_step);
		double r = rfilter.apply(rdata, rsample, info.time_step);

		//Mix
		lsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);
		rsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);

		lsample += l * fmin(0.5, preset.mix) * 2;
		rsample += r * fmin(0.5, preset.mix) * 2;
	}
}

WahWahEffect::~WahWahEffect() {

}

template<>
std::string get_effect_name<WahWahEffect>() {
	return "Wah-Wah";
}

template<>
EffectProgram* create_effect_program<WahWahEffect>() {
	return new WahWahProgram();
}

void WahWahProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on = tree.get<bool>("on", true);
	preset.auto_wah = tree.get<bool>("auto_wah", true);
	preset.mix = tree.get<double>("mix", 1);
	preset.cutoff = tree.get<double>("cutoff", 0.2);
	preset.resonance = tree.get<double>("resonance", 0.5);
	preset.amount = tree.get<double>("amount", 0.5);
}

boost::property_tree::ptree WahWahProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("auto_wah", preset.auto_wah);
	tree.put("mix", preset.mix);

	tree.put("cutoff", preset.cutoff);
	tree.put("resonance", preset.resonance);
	tree.put("amount", preset.amount);
	return tree;
}

void WahWahEffect::save_program(EffectProgram **prog) {
	WahWahProgram *p = dynamic_cast<WahWahProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new WahWahProgram();
	}
	p->ccs = cc.get_ccs();
	p->preset = preset;
	*prog = p;
}

void WahWahEffect::apply_program(EffectProgram *prog) {
	WahWahProgram *p = dynamic_cast<WahWahProgram*>(prog);
	//Create new
	if (p) {
		cc.set_ccs(p->ccs);
		preset = p->preset;
	} else {
		cc.set_ccs( { });
		preset = { };
	}
}
