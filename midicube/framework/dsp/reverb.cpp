/*
 * reverb.cpp
 *
 *  Created on: 16 Dec 2021
 *      Author: jojo
 */

#include "reverb.h"

double ReverbCombFilter::process(double in, double gain, unsigned int delay) {
	double out = this->delay.process();
	out += in;
	this->delay.add_isample(out * gain, delay);
	return out;
}

double ReverbAllPassFilter::process(double in, double gain, unsigned int delay) {
	double out = in + this->indelay.process() + this->delay.process();
	this->indelay.add_isample(out * -gain, delay);
	this->delay.add_isample(out * gain, delay - 20);
	return out;
}

double SchroederReverb::apply(double sample, const SchroederReverbData &data, const SampleInfo& info) {
	double out = 0;
	//Comb filters
	for (size_t i = 0; i < REVERB_COMB_FILTERS; ++i) {
		double comb_delay = (data.delay * comb_delay_mul[i]) * info.sample_rate;
		double decay = data.feedback * comb_decay_mul[i];
		out += comb_filters[i].process(sample, decay, comb_delay);
	}

	//All pass filters
	unsigned int allpass_delay = (this->allpass_delay) * info.sample_rate;
	for (size_t i = 0; i < REVERB_ALLPASS_FILTERS; ++i) {
		out = allpass_filters[i].process(sample, allpass_decay, allpass_delay);
	}
	//Apply
	sample /= 3.0;
	return sample;
}
