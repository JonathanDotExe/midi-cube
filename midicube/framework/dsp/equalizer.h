/*
 * equalizer.h
 *
 *  Created on: Nov 22, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_DSP_EQUALIZER_H_
#define MIDICUBE_FRAMEWORK_DSP_EQUALIZER_H_

#include <iostream>
#include <array>
#include "filter.h"

struct EqualizerPoint {
	double freq;
	double mul;
};

template<size_t N>
struct NBandEqualizerData {
	EqualizerPoint low;
	std::array<EqualizerPoint, N> mids;
	EqualizerPoint high;
};

template<size_t N>
class NBandEqualizer {
private:
	Filter low;
	std::array<Filter, N> filter;
	Filter high;

public:

	double apply(double sample, const NBandEqualizerData<N>) {
		return sample;
	}


};


#endif /* MIDICUBE_FRAMEWORK_DSP_EQUALIZER_H_ */
