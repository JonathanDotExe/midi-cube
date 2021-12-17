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
	if (delay < DELAY_BUFFER_SIZE) {
		buffer[(index + delay) % DELAY_BUFFER_SIZE] += sample;
	}
}

void DelayBuffer::add_isample(double sample, double delay) {
	unsigned int delay_int = (unsigned int) delay;
	double delay_frac = delay - delay_int;
	if (delay_int < DELAY_BUFFER_SIZE) {
		buffer[(index + delay_int) % DELAY_BUFFER_SIZE] += sample * (1 - delay_frac);
		if (delay_int + 1 < DELAY_BUFFER_SIZE) {
			buffer[(index + delay_int + 1) % DELAY_BUFFER_SIZE] += sample * delay_frac;
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
	++index;
	index %= DELAY_BUFFER_SIZE;
	return sample;
}

//PortamendoBuffer
PortamendoBuffer::PortamendoBuffer(double value) {
	this->next_value = value;
	this->value = value;
	this->step = 0;
}

double PortamendoBuffer::process(double time_step) {
	if (step) {
		double next = value + step * time_step;
		if ((next > next_value) != (value > next_value)) {
			value = next_value;
			step = 0;
		}
		else {
			value = next;
		}
	}
	return value;
}

double PortamendoBuffer::get() {
	return value;
}

void PortamendoBuffer::set(double value, double attack_step, double release_step) {
	next_value = value;
	if (this->value > value) {
		step = release_step;
	}
	else {
		step = attack_step;
	}
}

void LookbackDelayBuffer::process(double sample) {
	++index;
	index %= DELAY_BUFFER_SIZE;
	buffer[index] = sample;
}

double LookbackDelayBuffer::get_isample(double delay) {
	double sample = 0;
	unsigned int delay_int = (unsigned int) ceil(delay);
	double delay_frac = delay_int - delay;
	if (delay_int < DELAY_BUFFER_SIZE) {
		unsigned int delay_index = ((int) index - delay_int) % DELAY_BUFFER_SIZE;
		sample += buffer[delay_index] * (1 - delay_frac);
		if (delay_int + 1 < DELAY_BUFFER_SIZE) {
			unsigned int next_index= ((int) delay_index + 1) % DELAY_BUFFER_SIZE;
			sample += buffer[next_index] * delay_frac;
		}
	}
	return sample;
}
