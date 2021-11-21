/*
 * distortion.cpp
 *
 *  Created on: Nov 21, 2021
 *      Author: jojo
 */

#include "distortion.h"

inline double cubic_distortion(double sample) {
	if (sample < 0) {
		double t = sample + 1;
		sample = t * t * t - 1;
	}
	else {
		double t = sample - 1;
		sample = t * t * t + 1;
	}
	return sample;
}

double apply_distortion(double sample, double drive, DistortionType type) {
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
		sample = atan(sample * (1 + drive * 4));
	}
		break;
	case DistortionType::CUBIC_DISTORTION:
	{
		sample = cubic_distortion(sample * (0.3 + drive * 4.5));
	}
	break;
	case DistortionType::FUZZ_DISTORTION:
	{
		sample = cubic_distortion(cubic_distortion(cubic_distortion(sample * (0.3 + drive)))) / 3.0;
	}
	break;
	}
	return sample;
}

double AmplifierSimulation::apply(double sample,
		const AmplifierSimulationData &data, double time_step) {
	//Distortion
	sample = apply_distortion(sample, data.drive, data.type);

	//Low-pass
	FilterData f;
	f.type = FilterType::LP_24;
	f.cutoff = 200 + data.tone * 20000;
	sample = filter.apply(f, sample, time_step);

	//Gain
	double gain = data.post_gain + 1;
	sample *= gain;

	return sample;
}
