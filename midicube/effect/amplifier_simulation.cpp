/*
 * amplifier_simulation.cpp
 *
 *  Created on: Jan 7, 2021
 *      Author: jojo
 */

#include "amplifier_simulation.h"

AmplifierSimulationEffect::AmplifierSimulationEffect() {

}

static double apply_distortion(double sample, double drive, DistortionType type) {
	switch (type) {
	/*case DistortionType::TUBE_AMP_DISTORTION:
	{
		double a = sin((drive * 100.0 + 1)/102 * (M_PI/2.0));
		double k = 2 * a/(1 - a);
		sample = (1 + k) * sample / (1 + k * abs(sample));
	}
		break;*/
	case DistortionType::DIGITAL_DISTORTION:
	{
		double clip = (1 - drive);
		sample = fmax(fmin(sample, clip), -clip);
		sample *= clip ? 1/clip : 0;
	}
		break;
	case DistortionType::POLYNOMAL_DISTORTION:
	{
		sample -= (sample * sample * sample) * drive;
	}
		break;
	case DistortionType::ARCTAN_DISTORTION:
	{
		sample = 2 / M_PI * atan(sample * (1.5 + drive * 2));
	}
		break;
	}
	return sample;
}

void AmplifierSimulationEffect::apply(double &lsample, double &rsample, AmplifierSimulationPreset &preset, SampleInfo &info) {
	if (preset.on) {
		//Gain
		double gain = preset.boost + 1;
		lsample *= gain;
		rsample *= gain;

		//Distortion
		lsample = apply_distortion(lsample, preset.drive, preset.type);
		rsample = apply_distortion(rsample, preset.drive, preset.type);

		//Low-pass
		FilterData data;
		data.type = FilterType::LP_24;
		data.cutoff = 200 + preset.tone * 20000;
		lsample = lfilter.apply(data, lsample, info.time_step);
		rsample = rfilter.apply(data, rsample, info.time_step);
	}
}

AmplifierSimulationEffect::~AmplifierSimulationEffect() {

}
