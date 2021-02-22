/*
 * filter.cpp
 *
 *  Created on: 26 Aug 2020
 *      Author: jojo
 */


#include <cmath>
#include <iostream>
#include "filter.h"


const double TWO_POLE_FACTOR = 1.0/sqrt(pow(2, 1/2.0) - 1);
const double FOUR_POLE_FACTOR = 1.0/sqrt(pow(2, 1/4.0) - 1);

/**
 * To convert frequency into filter cutoff use time_step/(rc+time_step)
 */

//Filter
double Filter::apply (FilterData& data, double sample, double time_step) {
	//Cutoff pole factor
	double factor = 1;
	if (data.type % 2 == 0) {
		factor = TWO_POLE_FACTOR;
	}
	else {
		factor = FOUR_POLE_FACTOR;
	}
	double cutoff = cutoff_to_factor(data.cutoff * factor, time_step);
	double feedback = data.resonance + data.resonance/(1 - cutoff);
	//Update buffers
	switch (data.type) {
	case FilterType::LP_12:
	case FilterType::LP_24:
	case FilterType::BP_12:
	case FilterType::BP_24:
		//Low-pass poles
		pole1 += cutoff * (sample - pole1 + feedback * (pole1 - pole2));
		pole2 += cutoff * (pole1 - pole2);
		pole3 += cutoff * (pole2 - pole3);
		pole4 += cutoff * (pole3 - pole4);
		break;
	case FilterType::HP_12:
	case FilterType::HP_24:
		//High pass poles
		pole1 = cutoff * (sample - last_pole1 + pole1 + feedback * (pole1 - pole2));
		pole2 = cutoff * (pole1 - last_pole2 + pole2);
		pole3 = cutoff * (pole2 - last_pole3 + pole3);
		pole4 = cutoff * (pole3 - last_pole4 + pole4);

		last_pole1 = pole1;
		last_pole2 = pole2;
		last_pole3 = pole3;
		last_pole4 = pole4;
	}
	switch (data.type) {
	case FilterType::LP_12:
		return pole2;
	case FilterType::LP_24:
		return pole4;
	case FilterType::HP_12:
		return sample - pole2;
	case FilterType::HP_24:
		return sample - pole4;
	case FilterType::BP_12:
		return pole1 - pole2;
	case FilterType::BP_24:
		return pole1 - pole4;
	}
	return sample;
}
