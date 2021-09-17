/*
 * pcm.cpp
 *
 *  Created on: Jun 3, 2021
 *      Author: jojo
 */

#include "pcm.h"

PCMOscilator::PCMOscilator() {
}

double PCMOscilator::process(double freq, double time_step, PCMOscilatorData &data) {
	double loop_duration = data.zone->loop_end - data.zone->loop_start;
	//Loop
	while (time > data.zone->loop_end) {
		time -= loop_duration;
	}
	double sample = data.zone->sample.isample(0, time, 1/time_step);
	//Time
	time += freq/data.zone->freq * time_step;
	return sample;
}

void PCMOscilator::reset() {
	time = 0;
}

PCMOscilator::~PCMOscilator() {
}
