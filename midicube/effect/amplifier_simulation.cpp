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
	double gain = preset.boost * 99 + 1;
	lsample *= gain;
	rsample *= gain;

	//Distortion
	lsample = apply_distortion(lsample, preset.drive, preset.type);
	rsample = apply_distortion(lsample, preset.drive, preset.type);
}

AmplifierSimulationEffect::~AmplifierSimulationEffect() {

}
