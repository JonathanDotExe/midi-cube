/*
 * distortion.cpp
 *
 *  Created on: Nov 21, 2021
 *      Author: jojo
 */

#include "distortion.h"


double TubeAmpTriode::apply(double sample, const TubeAmpTriodeData &data,
		double time_step) {
	sample = -apply_distortion(sample, data.drive, data.type);
	FilterData h{FilterType::HP_24, data.highpass_cutoff};
	sample = highfilter.apply(h, sample, time_step);
	FilterData l{FilterType::HP_24, data.lowshelf_cutoff};
	sample += lowfilter.apply(l, sample, time_step) * data.lowshelf_boost;
	sample /= 1 + data.lowshelf_boost;
	return sample;
}
