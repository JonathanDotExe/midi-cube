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

inline double hard_clip_waveshaper(double sample);

inline double polynomial_waveshaper(double sample, double drive);

inline double arctan_waveshaper(double sample, double drive);

inline double cubic_waveshaper(double sample);

inline double cubic_fuzz_waveshaper(double sample);

inline double soft_clip_waveshaper(double sample);

inline double tanh_waveshaper(double sample, double drive);

inline double sigmoid_waveshaper(double sample);

double apply_distortion(double sample, double drive, DistortionType type);

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
	double tone = 0.8;
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
