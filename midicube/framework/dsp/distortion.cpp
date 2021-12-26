/*
 * distortion.cpp
 *
 *  Created on: Nov 21, 2021
 *      Author: jojo
 */

#include "distortion.h"

inline double cubic_waveshaper(double sample) {
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

inline double hard_clip_waveshaper(double sample) {
	return fmax(fmin(sample, 1), -1);
}

inline double polynomial_waveshaper(double sample, double drive) {
	return sample - (sample * sample * sample) * drive;
}

inline double arctan_waveshaper(double sample, double drive) {
	return atan(sample * drive)/atan(sample);
}

inline double cubic_fuzz_waveshaper(double sample) {
	return cubic_waveshaper(cubic_waveshaper(cubic_waveshaper(sample))) / 3.0;
}

inline double soft_clip_waveshaper(double sample) {
	return 3*sample/2 * (1 - sample*sample/3);
}

inline double tanh_waveshaper(double sample, double drive) {
	return tanh(sample * drive)/tanh(sample);
}

inline double sigmoid_waveshaper(double sample) {
	return 2.0/(1.0 + pow(M_E, -sample)) - 1;
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
		sample = hard_clip_waveshaper(sample * (1 + 9 * drive));
	}
		break;
	case DistortionType::POLYNOMAL_DISTORTION:
	{
		sample = polynomial_waveshaper(sample, drive);
	}
		break;
	case DistortionType::ARCTAN_DISTORTION:
	{
		sample = arctan_waveshaper(sample, (1 + drive * 4));
	}
		break;
	case DistortionType::CUBIC_DISTORTION:
	{
		sample = cubic_waveshaper(sample * (0.3 + drive * 4.5));
	}
	break;
	case DistortionType::FUZZ_DISTORTION:
	{
		sample = cubic_fuzz_waveshaper((sample * (0.3 + drive)));
	}
	break;
	case DistortionType::SOFT_CLIP_DISTORTION:
	{
		sample = soft_clip_waveshaper((sample * (1 + 4 * drive)));
	}
	break;
	case DistortionType::TANH_DISTORTION:
	{
		sample = tanh_waveshaper(sample, (1 + 4 * drive));
	}
	break;
	case DistortionType::SIGMOID_DISTORTION:
	{
		sample = sigmoid_waveshaper((sample * (1 + 4 * drive)));
	}
	break;
	}
	return sample;
}

double TubeAmpTriode::apply(double sample, const TubeAmpTriodeData &data,
		double time_step) {
	sample = -apply_distortion(sample, data.drive, data.type);
	FilterData h{FilterType::HP_24, data.highpass_cutoff};
	sample = highfilter.apply(h, sample, time_step);
	FilterData l{FilterType::HP_24, data.lowshelf_cutoff};
	sample += lowfilter.apply(l, sample, time_step) * data.lowshelf_boost;
	return sample;
}
