/*
 * synthesis.cpp
 *
 *  Created on: 20 Jun 2020
 *      Author: jojo
 */

#include "synthesis.h"
#include <cmath>
#include <iostream>


//DelayBuffer
void DelayBuffer::add_sample(double sample, unsigned int delay) {
	if (delay < buffer.size()) {
		buffer[(index + delay) % buffer.size()] += sample;
	}
}

void DelayBuffer::add_isample(double sample, double delay) {
	unsigned int delay_int = (unsigned int) delay;
	double delay_frac = delay - delay_int;
	if (delay_int < buffer.size()) {
		buffer[(index + delay_int) % buffer.size()] += sample * (1 - delay_frac);
		if (delay_int + 1 < buffer.size()) {
			buffer[(index + delay_int + 1) % buffer.size()] += sample * delay_frac;
		}
	}
}

void DelayBuffer::add_sample(double sample, unsigned int delay, unsigned int repetition, unsigned int rep_delay, double factor) {
	for (unsigned int i = 0; i < repetition; ++i) {
		add_sample(sample, delay);
		sample *= factor;
		delay += rep_delay;
	}
}

double DelayBuffer::process() {
	double sample = buffer[index];
	buffer[index] = 0;
	index++;
	index %= buffer.size();
	return sample;
}

//PortamendoBuffer
PortamendoBuffer::PortamendoBuffer(double value, double slope_time) {
	this->last_value = value;
	this->value = value;
	this->last_time = 0;
	this->slope_time = slope_time;
}

double PortamendoBuffer::get(double time) {
	if (last_time + slope_time <= time) {
		return value;
	}
	else {
		double prog = (time - last_time)/slope_time;
		return last_value * (1 - prog) + value * prog;
	}
}

void PortamendoBuffer::set(double value, double time, double slope_time) {
	last_value = get(time);
	last_time = time;
	this->value = value;
	this->slope_time = slope_time;
}


