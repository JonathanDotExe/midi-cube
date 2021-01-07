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
	case DistortionType::TUBE_AMP_DISTORTION:
	{
		double a = sin((drive * 100.0 + 1)/101 * M_PI/2.0);
		double k = 2 * a/(1 - a);
		sample = (1 + k) * sample / (1 + k * abs(sample));
	}
		break;
	case DistortionType::DIGITAL_DISTORTION:
	{
		double clip = (1 - drive);
		sample = fmax(fmin(sample, clip), -clip);
		sample *= clip ? 1/clip : 0;
	}
		break;
	case DistortionType::SOFT_CLIPPING_1:
	{
		sample -= (sample * sample * sample) * drive;
	}
		break;
	case DistortionType::SOFT_CLIPPING_2:
	{
		sample = 2 / M_PI * atan(sample * drive * 10);
	}
		break;
	}
	return sample;
}

void AmplifierSimulationEffect::apply(double &lsample, double &rsample, AmplifierSimulationPreset &preset, SampleInfo &info) {
	//Gain
	double gain = preset.boost + 1;
	lsample *= gain;
	rsample *= gain;

	//Distortion
	lsample = apply_distortion(lsample, preset.drive, preset.type);
	rsample = apply_distortion(lsample, preset.drive, preset.type);
}

AmplifierSimulationEffect::~AmplifierSimulationEffect() {

}
