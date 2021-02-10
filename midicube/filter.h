/*
 * filter.h
 *
 *  Created on: 26 Aug 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_FILTER_H_
#define MIDICUBE_FILTER_H_
#include <array>
#include <type_traits>
#include "audio.h"
#include <cmath>

inline double cutoff_to_rc(double cutoff) {
	return 1.0/(2 * M_PI * cutoff);
}

inline double rc_to_cutoff_factor(double rc, double time_step) {
	return time_step/(rc+time_step);
}

inline double cutoff_to_factor(double cutoff, double time_step) {
	return rc_to_cutoff_factor(cutoff_to_rc(cutoff), time_step);
}

inline double factor_to_cutoff(double cutoff, double time_step) {
	return 1/(2*M_PI*(time_step/cutoff-time_step));
}

enum FilterType {
	LP_12, LP_24, HP_12, HP_24, BP_12, BP_24
};

struct FilterData {
	FilterType type;
	double cutoff = 1;
	double resonance = 0;
};

class Filter {
private:

public:
	double pole1 = 0;
	double pole2 = 0;
	double pole3 = 0;
	double pole4 = 0;
	double apply (FilterData& data, double sample, double time_step);

};

#endif /* MIDICUBE_FILTER_H_ */
