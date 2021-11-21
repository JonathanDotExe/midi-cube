/*
 * distortion.h
 *
 *  Created on: Nov 21, 2021
 *      Author: jojo
 */

#include <cmath>
#include "filter.h"

enum DistortionType {
	DIGITAL_DISTORTION, POLYNOMAL_DISTORTION, ARCTAN_DISTORTION, CUBIC_DISTORTION, FUZZ_DISTORTION
};

inline double cubic_distortion(double sample);

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
