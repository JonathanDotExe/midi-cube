/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "chorus.h"
#include <cmath>

ChorusEffect::ChorusEffect() {

}

void ChorusEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		double l = ldelay.process();
		double r = rdelay.process();

		//Delay
		AnalogOscilatorData data = {preset.vibrato_waveform};
		osc.process(preset.vibrato_rate, info.time_step, data);
		double del = (1 + osc.carrier(preset.vibrato_rate, info.time_step, data) * preset.vibrato_depth) * preset.delay * 0.2 * info.sample_rate;
		ldelay.add_isample(lsample, del);
		rdelay.add_isample(rsample, del);

		//Mix
		lsample = lsample * (1 - preset.mix) + l * preset.mix;
		rsample = rsample * (1 - preset.mix) + r * preset.mix;
	}
}

ChorusEffect::~ChorusEffect() {

}

template<>
std::string get_effect_name<ChorusEffect>() {
	return "Chorus";
}

