/*
 * filter.cpp
 *
 *  Created on: 26 Aug 2020
 *      Author: jojo
 */


#include <cmath>
#include "filter.h"


double cutoff_to_rc(double cutoff) {
	return 1.0/(2 * M_PI * cutoff);
}

static double rc_lowpass(FilterCache& c, double cutoff, double sample, double time_step) {
	double rc = cutoff_to_rc(cutoff);
	double filtered = 0;
	double a = time_step / (rc + time_step);

	filtered = a * sample + (1 - a) * c.last_filtered;

	c.last_filtered = filtered;
	return filtered;
}

static double rc_highpass(FilterCache& c, double cutoff, double sample, double time_step) {
	double filtered = 0;
	double rc = cutoff_to_rc(cutoff);
	double a = rc / (rc + time_step);

	if (c.started) {
		filtered = a * c.last_filtered + a * (sample - c.last);
	}
	else {
		filtered = sample;
		c.started = true;
	}

	c.last = sample;
	c.last_filtered = filtered;
	return filtered;
}

//Filter
double Filter::apply (FilterData& data, double sample, double time_step) {
	size_t rep = 4;
	switch (data.type) {
	case FilterType::HP_12:
		rep = 2;
	case FilterType::HP_24:

		break;
	case FilterType::LP_12:
		rep = 2;
	case FilterType::LP_24:
		break;
	}
	return 0;
}
