/*
 * distortion.h
 *
 *  Created on: Nov 21, 2021
 *      Author: jojo
 */

#include <cmath>
#include "filter.h"
#include "equalizer.h"

#ifndef MIDICUBE_FRAMEWORK_DSP_DISTORTION_H_
#define MIDICUBE_FRAMEWORK_DSP_DISTORTION_H_

#define TUBE_AMP_TRIODE_COUNT 4

enum DistortionType {
	DIGITAL_DISTORTION, POLYNOMAL_DISTORTION, ARCTAN_DISTORTION, CUBIC_DISTORTION, FUZZ_DISTORTION, SOFT_CLIP_DISTORTION, TANH_DISTORTION, SIGMOID_DISTORTION
};

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

inline double arctan_waveshaper(double sample) {
	return atan(sample);
}

inline double cubic_fuzz_waveshaper(double sample) {
	return cubic_waveshaper(cubic_waveshaper(cubic_waveshaper(sample))) / 3.0;
}

inline double soft_clip_waveshaper(double sample) {
	return 3*sample/2 * (1 - sample*sample/3);
}

inline double tanh_waveshaper(double sample) {
	return tanh(sample);
}

inline double sigmoid_waveshaper(double sample) {
	return 2.0/(1.0 + pow(M_E, -sample)) - 1;
}

inline double apply_distortion(double sample, double drive, DistortionType type) {
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
		sample = hard_clip_waveshaper(sample * (1 + drive));
	}
		break;
	case DistortionType::POLYNOMAL_DISTORTION:
	{
		sample = polynomial_waveshaper(sample, drive);
	}
		break;
	case DistortionType::ARCTAN_DISTORTION:
	{
		sample = arctan_waveshaper(sample * (1 + drive));
	}
		break;
	case DistortionType::CUBIC_DISTORTION:
	{
		sample = cubic_waveshaper(sample * (0.3 + drive));
	}
	break;
	case DistortionType::FUZZ_DISTORTION:
	{
		sample = cubic_fuzz_waveshaper((sample * (0.3 + drive)));
	}
	break;
	case DistortionType::SOFT_CLIP_DISTORTION:
	{
		sample = soft_clip_waveshaper((sample * (1 + drive)));
	}
	break;
	case DistortionType::TANH_DISTORTION:
	{
		sample = tanh_waveshaper(sample * (1 + drive));
	}
	break;
	case DistortionType::SIGMOID_DISTORTION:
	{
		sample = sigmoid_waveshaper((sample * (1 + drive)));
	}
	break;
	}
	return sample;
}

struct TubeAmpTriodeData {
	double drive = 0;
	DistortionType type = TANH_DISTORTION;
	double highpass_cutoff = 1.0;
	double lowshelf_cutoff = 80;
	double lowshelf_boost = 0;
};


class TubeAmpTriode {
private:
	Filter lowfilter;
	Filter highfilter;

public:
	double apply(double sample, const TubeAmpTriodeData& data, double time_step);

};

template<size_t N>
struct AmplifierSimulationData {
	TubeAmpTriodeData triode;
	double post_gain = 0;
	double tone = 0.9;
	NBandEqualizerData<N> eq;
};

template<size_t N>
class AmplifierSimulation {
	std::array<TubeAmpTriode, TUBE_AMP_TRIODE_COUNT> triodes;
	NBandEqualizer<N> eq;
	Filter filter;
	
public:
	double apply(double sample, const AmplifierSimulationData<N>& data, double time_step);
};

template<size_t N>
double AmplifierSimulation<N>::apply(double sample,
		const AmplifierSimulationData<N> &data, double time_step) {
	//Distortion
	for (size_t i = 0; i < TUBE_AMP_TRIODE_COUNT; ++i) {
		//EQ
		if (i == TUBE_AMP_TRIODE_COUNT - 1) {
			sample = eq.apply(sample, data.eq, time_step);
		}
		sample = triodes[i].apply(sample, data.triode, time_step);
	}
	sample /= (1 + data.triode.drive * TUBE_AMP_TRIODE_COUNT);

	//Low-pass
	FilterData f;
	f.type = FilterType::LP_24;
	f.cutoff = scale_cutoff(data.tone);
	sample = filter.apply(f, sample, time_step);

	//Gain
	double gain = data.post_gain + 1;
	sample *= gain;

	return sample;
}

#endif /* MIDICUBE_FRAMEWORK_DSP_DISTORTION_H_ */
