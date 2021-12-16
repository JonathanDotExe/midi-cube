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

double SchroederReverb::apply(double sample, const SchroederReverbData &data) {
	//Comb filters
	for (size_t i = 0; i < REVERB_COMB_FILTERS; ++i) {
		double comb_delay = (preset.delay * comb_delay_mul[i]) * info.sample_rate;
		double decay = preset.decay * comb_decay_mul[i];
		l += lcomb_filters[i].process(outputs[0], decay, comb_delay * (1 - preset.stereo * 0.2));
		r += rcomb_filters[i].process(outputs[1], decay, comb_delay * (1 + preset.stereo * 0.2));
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

}
