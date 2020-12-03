/*
 * filter.cpp
 *
 *  Created on: 26 Aug 2020
 *      Author: jojo
 */


#include <cmath>
#include <iostream>
#include "filter.h"


/**
 * To convert frequency into filter cutoff use time_step/(rc+time_step)
 */

//Filter
double Filter::apply (FilterData& data, double sample, double time_step) {
	double cutoff = fmin(data.cutoff, 0.9999999999);
	double feedback = data.resonance + data.resonance/(1 - cutoff);
	//Update buffers
	pole1 += data.cutoff * (sample - pole1 + feedback * (pole1 - pole2));
	pole2 += data.cutoff * (pole1 - pole2);
	pole3 += data.cutoff * (pole2 - pole3);
	pole4 += data.cutoff * (pole3 - pole4);
	switch (data.type) {
	case FilterType::LP_12:
		return pole2;
	case FilterType::LP_24:
		return pole4;
	case FilterType::HP_12:
		return sample - pole2;
	case FilterType::HP_24:
		return sample - pole4;
	case FilterType::LP_12:
		return pole1 - pole2;
	case FilterType::LP_24:
		return pole1 - pole4;
	}
	return sample;
}
