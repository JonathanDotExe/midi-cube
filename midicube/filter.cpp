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

/**
 * To convert frequency into filter cutoff use time_step/(rc+time_step)
 */

//Filter
double Filter::apply (FilterData& data, double sample, double time_step) {
	//Update buffers
	pole1 += data.cutoff * (sample - pole1);
	pole2 += data.cutoff * (pole1 - pole2);
	pole3 += data.cutoff * (pole2 - pole3);
	pole4 += data.cutoff * (pole3 - pole4);
	switch (data.type) {
	case FilterType::LP_12:
		sample = pole2;
		break;
	case FilterType::LP_24:
		sample = pole4;
		break;
	case FilterType::HP_12:
		sample = sample - pole2;
		break;
	case FilterType::HP_24:
		sample = sample - pole4;
		break;
	}
	return sample;
}
