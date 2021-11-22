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
	FilterType type;
};

template<size_t N>
struct NBandEqualizerData {
	std::array<EqualizerPoint, N> points;
};

template<size_t N>
class NBandEqualizer {
private:
	std::array<Filter, N> filter;

public:

	double apply(double sample, const NBandEqualizerData<N>& data, double time_step) {
		double out = sample;
		for (size_t i = 0; i < N; ++i) {
			FilterData f = {data.points[i].type, data.points[i].freq};
			double filtered = filter[i].apply(data, sample, time_step);

			//Apply
			out += filtered * data.points[i].mul;
		}
		return out;
	}


};


#endif /* MIDICUBE_FRAMEWORK_DSP_EQUALIZER_H_ */
