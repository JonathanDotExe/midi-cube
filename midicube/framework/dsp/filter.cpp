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
const double BP_6_BOOST = db_to_amp(1.5);
const double BP_12_BOOST = db_to_amp(3);
const double BP_18_BOOST = db_to_amp(4.5);
const double BP_24_BOOST = db_to_amp(6);

/**
 * To convert frequency into filter cutoff use time_step/(rc+time_step)
 */

//Filter
double Filter::apply (FilterData& data, double sample, double time_step) {
	switch (data.type) {
	//Low pass
	case FilterType::LP_6:
	{
		do_lowpass(sample, data.cutoff, data.resonance, 2, time_step);
		return poles[0].pole;
	}
	case FilterType::LP_12:
	{
		do_lowpass(sample, data.cutoff * TWO_POLE_FACTOR, data.resonance, 2, time_step);
		return poles[1].pole;
	}
	case FilterType::LP_18:
		do_lowpass(sample, data.cutoff * THREE_POLE_FACTOR, data.resonance, 3, time_step);
		return poles[2].pole;
	case FilterType::LP_24:
		do_lowpass(sample, data.cutoff * FOUR_POLE_FACTOR, data.resonance, 4, time_step);
		return poles[3].pole;
	//Low pass BP
	case FilterType::LP_12_BP:
		do_lowpass(sample, data.cutoff, data.resonance, 2, time_step);
		return poles[0].pole - poles[1].pole;
	case FilterType::LP_24_BP:
	{
		do_lowpass(sample, data.cutoff, data.resonance, 4, time_step);
		return poles[0].pole - poles[3].pole;
	}
	//High pass
	case FilterType::HP_6:
	{
		do_highpass(sample, data.cutoff, data.resonance, 2, time_step);
		return poles[0].pole;
	}
	case FilterType::HP_12:
	{
		do_highpass(sample, data.cutoff * TWO_POLE_FACTOR, data.resonance, 2, time_step);
		return poles[1].pole;
	}
	case FilterType::HP_18:
		do_highpass(sample, data.cutoff * THREE_POLE_FACTOR, data.resonance, 3, time_step);
		return poles[2].pole;
	case FilterType::HP_24:
		do_highpass(sample, data.cutoff * FOUR_POLE_FACTOR, data.resonance, 4, time_step);
		return poles[3].pole;
	//BP
	case BP_6:
		do_lowpass(sample, data.cutoff, data.resonance, 1, time_step);
		do_highpass(poles[0].pole, data.cutoff, 0, 1, time_step, 4);
		return poles[4].pole * BP_6_BOOST;
	case BP_12:
		do_lowpass(sample, data.cutoff * TWO_POLE_FACTOR, data.resonance, 2, time_step);
		do_highpass(poles[1].pole, data.cutoff * TWO_POLE_FACTOR, 0, 2, time_step, 4);
		return poles[5].pole * BP_12_BOOST;
	case BP_18:
		do_lowpass(sample, data.cutoff * THREE_POLE_FACTOR, data.resonance, 3, time_step);
		do_highpass(poles[2].pole, data.cutoff * THREE_POLE_FACTOR, 0, 3, time_step, 4);
		return poles[6].pole * BP_18_BOOST;
	case BP_24:
		do_lowpass(sample, data.cutoff * FOUR_POLE_FACTOR, data.resonance, 4, time_step);
		do_highpass(poles[3].pole, data.cutoff * FOUR_POLE_FACTOR, 0, 4, time_step, 4);
		return poles[7].pole * BP_24_BOOST;
	}

	return sample;
}
