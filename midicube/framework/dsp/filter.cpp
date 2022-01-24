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
const double THREE_POLE_FACTOR = 1.0/sqrt(pow(2, 1/3.0) - 1);
const double FOUR_POLE_FACTOR = 1.0/sqrt(pow(2, 1/4.0) - 1);
const double BP_12_BOOST = db_to_amp(3);
const double BP_18_BOOST = db_to_amp(4.5);
const double BP_24_BOOST = db_to_amp(6);

/**
 * To convert frequency into filter cutoff use time_step/(rc+time_step)
 */

//Filter
double Filter::apply (FilterData& data, double sample, double time_step) {
	switch (data.type) {
	//Low pass algorithm
	case FilterType::LP_6:
	case FilterType::LP_12:
	case FilterType::LP_24:
	case FilterType::LP_12_BP:
	case FilterType::LP_24_BP:
	{
		//Cutoff
		double factor = 1;
		if (data.type == LP_6) {
			factor = 1;
		}
		else if (data.type % 2 == 0) {
			factor = TWO_POLE_FACTOR;
		}
		else {
			factor = FOUR_POLE_FACTOR;
		}
		double cutoff = cutoff_to_factor(data.cutoff * factor, time_step);
		double feedback = data.resonance + data.resonance/(1 - cutoff);
		//Update buffer
		pole1 += cutoff * (sample - pole1 + feedback * (pole1 - pole2));
		pole2 += cutoff * (pole1 - pole2);
		pole3 += cutoff * (pole2 - pole3);
		pole4 += cutoff * (pole3 - pole4);

		//Values
		switch (data.type) {
		case FilterType::LP_6:
			return pole1;
		case FilterType::LP_12:
			return pole2;
		case FilterType::LP_24:
			return pole4;
		case FilterType::LP_12_BP:
			return pole1 - pole2;
		case FilterType::LP_24_BP:
			return pole1 - pole4;
		default:
			break;
		}
	}
		break;
	//High pass algorithm
	case FilterType::HP_6:
	case FilterType::HP_12:
	case FilterType::HP_24:
	{
		//Cutoff
		double factor = 1;
		if (data.type == HP_6) {
			factor = 1;
		}
		else if (data.type % 2 == 0) {
			factor = TWO_POLE_FACTOR;
		}
		else {
			factor = FOUR_POLE_FACTOR;
		}
		double cutoff = cutoff_to_highpass_factor(data.cutoff * factor, time_step);
		double feedback = data.resonance + data.resonance/(cutoff);
		//High pass poles
		sample +=  feedback * (pole1 - pole2);
		pole1 = cutoff * (sample - last_pole1 + pole1);
		pole2 = cutoff * (pole1 - last_pole2 + pole2);
		pole3 = cutoff * (pole2 - last_pole3 + pole3);
		pole4 = cutoff * (pole3 - last_pole4 + pole4);

		last_pole1 = sample;
		last_pole2 = pole1;
		last_pole3 = pole2;
		last_pole4 = pole3;

		switch (data.type) {
		case FilterType::HP_6:
			return pole1;
		case FilterType::HP_12:
			return pole2;
		case FilterType::HP_24:
			return pole4;
		default:
			break;
		}
	}
		break;
	case BP_12:
		//Cutoff
		double factor = TWO_POLE_FACTOR;
		double cutoff = cutoff_to_factor(data.cutoff * factor, time_step);
		double hp_cutoff = cutoff_to_highpass_factor(data.cutoff * factor, time_step);
		double feedback = data.resonance + data.resonance/(1 - cutoff);
		//Update lowpass buffer
		pole1 += cutoff * (sample - pole1 + feedback * (pole1 - pole2));
		pole2 += cutoff * (pole1 - pole2);
		//Update highpass buffer
		pole3 = hp_cutoff * (pole2 - last_pole3 + pole3);
		pole4 = hp_cutoff * (pole3 - last_pole4 + pole4);

		last_pole3 = pole2;
		last_pole4 = pole3;

		//Values
		return pole4 * BP_12_BOOST;
	}

	return sample;
}
