/*
 * reverb.cpp
 *
 *  Created on: 20 Apr 2021
 *      Author: jojo
 */

#include "reverb.h"

#include <cmath>

double CombFilter::process(double in, double gain, unsigned int delay) {
	double out = this->delay.process();
	out += in;
	this->delay.add_sample(out * gain, delay);
	return out;
}

double AllPassFilter::process(double in, double gain, unsigned int delay) {
	double out = in + this->indelay.process() + this->delay.process();
	this->indelay.add_sample(out * -gain, delay);
	this->delay.add_sample(out * gain, delay - 20);
	return out;
}

ReverbEffect::ReverbEffect() {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.delay);
	cc.add_binding(&preset.decay);
	cc.add_binding(&preset.mix);
	cc.add_binding(&preset.tone);
	cc.add_binding(&preset.resonance);
	cc.add_binding(&preset.stereo);
}

void ReverbEffect::apply(double &lsample, double &rsample, SampleInfo &info) {
	if (preset.on) {
		double l = 0;
		double r = 0;

		//Comb filters
		for (size_t i = 0; i < REVERB_COMB_FILTERS; ++i) {
			double comb_delay = (preset.delay * comb_delay_mul[i]) * info.sample_rate;
			double decay = preset.decay * comb_decay_mul[i];
			l += lcomb_filters[i].process(lsample, decay, comb_delay * (1 - preset.stereo * 0.2));
			r += rcomb_filters[i].process(rsample, decay, comb_delay * (1 + preset.stereo * 0.2));
		}

		//All pass filters
		unsigned int allpass_delay = (this->allpass_delay) * info.sample_rate;
		for (size_t i = 0; i < REVERB_ALLPASS_FILTERS; ++i) {
			l = lallpass_filters[i].process(l, allpass_decay, allpass_delay);
			r = rallpass_filters[i].process(r, allpass_decay, allpass_delay);
		}
		//Apply
		l /= 3.0;
		r /= 3.0;
		//Lowpass
		FilterData data = {FilterType::LP_12, scale_cutoff(preset.tone), preset.resonance};
		l = lfilter.apply(data, l, info.time_step);
		r = lfilter.apply(data, r, info.time_step);
		//Mix
		lsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);
		rsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);

		lsample += l * fmin(0.5, preset.mix) * 2;
		rsample += r * fmin(0.5, preset.mix) * 2;
	}
}

void ReverbEffect::save_program(EffectProgram **prog) {
	ReverbProgram* p = dynamic_cast<ReverbProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new ReverbProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void ReverbEffect::apply_program(EffectProgram *prog) {
	ReverbProgram* p = dynamic_cast<ReverbProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}

ReverbEffect::~ReverbEffect() {
}

template<>
std::string get_effect_name<ReverbEffect>() {
	return "Reverb";
}

template <>
EffectProgram* create_effect_program<ReverbEffect>() {
	return new ReverbProgram();
}


void ReverbProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on.load(tree, "on", true);
	preset.delay.load(tree, "delay", 0.2);
	preset.decay.load(tree, "decay", 0.7);
	preset.mix.load(tree, "mix", 0.5);

	preset.tone.load(tree, "tone", 0.35);
	preset.resonance.load(tree, "resonance", 0.0);
	preset.stereo.load(tree, "stereo", 0.0);
}

boost::property_tree::ptree ReverbProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.add_child("on", preset.on.save());
	tree.add_child("delay", preset.delay.save());
	tree.add_child("decay", preset.decay.save());
	tree.add_child("mix", preset.mix.save());

	tree.add_child("tone", preset.tone.save());
	tree.add_child("resonance", preset.resonance.save());
	tree.add_child("stereo", preset.stereo.save());
	return tree;
}
