/*
 * distortion.h
 *
 *  Created on: Nov 21, 2021
 *      Author: jojo
 */

#include <cmath>
#include "filter.h"

#ifndef MIDICUBE_FRAMEWORK_DSP_DISTORTION_H_
#define MIDICUBE_FRAMEWORK_DSP_DISTORTION_H_

enum DistortionType {
	DIGITAL_DISTORTION, POLYNOMAL_DISTORTION, ARCTAN_DISTORTION, CUBIC_DISTORTION, FUZZ_DISTORTION, SOFT_CLIP_DISTORTION, TANH_DISTORTION, SIGMOID_DISTORTION
};

inline double hard_clip_waveshaper(double sample);

inline double polynomial_waveshaper(double sample, double drive);

inline double arctan_waveshaper(double sample);

inline double cubic_waveshaper(double sample);

inline double cubic_fuzz_waveshaper(double sample);

inline double soft_clip_waveshaper(double sample);

inline double tanh_waveshaper(double sample);

inline double sigmoid_waveshaper(double sample);

double apply_distortion(double sample, double drive, DistortionType type);

struct AmplifierSimulationData {
	double drive = 0;
	double post_gain = 0;
	double tone = 0.6;
	DistortionType type = ARCTAN_DISTORTION;
};

class AmplifierSimulation {
	Filter filter;
	
public:
	double apply(double sample, const AmplifierSimulationData& data, double time_step);
};

#endif /* MIDICUBE_FRAMEWORK_DSP_DISTORTION_H_ */
