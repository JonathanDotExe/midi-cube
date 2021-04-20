/*
 * reverb.cpp
 *
 *  Created on: 20 Apr 2021
 *      Author: jojo
 */

#include "reverb.h"

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
}

void ReverbEffect::apply(double &lsample, double &rsample, SampleInfo &info) {
	if (preset.on) {
		double l = 0;
		double r = 0;

		//Comb filters
		for (size_t i = 0; i < REVERB_COMB_FILTERS; ++i) {
			unsigned int comb_delay = (preset.delay + preset.comb_delay_diff[i]) * info.sample_rate;
			double decay = preset.decay + preset.comb_decay_diff[i];
			l += lcomb_filters[i].process(lsample, decay, comb_delay);
			r += rcomb_filters[i].process(rsample, decay, comb_delay);
		}
		//Mix
		l = lsample * (1 - preset.mix) + l * preset.mix;
		r = rsample * (1 - preset.mix) + r * preset.mix;
		//All pass filters
		unsigned int allpass_delay = (preset.allpass_delay) * info.sample_rate;
		for (size_t i = 0; i < REVERB_ALLPASS_FILTERS; ++i) {
			l = lallpass_filters[i].process(l, preset.allpass_decay, allpass_delay);
			r = rallpass_filters[i].process(r, preset.allpass_decay, allpass_delay);
		}
		//Apply
		lsample = l;
		rsample = r;
	}
}

void ReverbEffect::save_program(EffectProgram **prog) {
}

void ReverbEffect::apply_program(EffectProgram *prog) {
}

ReverbEffect::~ReverbEffect() {
}

template<>
std::string get_effect_name<ReverbEffect>() {
	return "Reverb";
}
