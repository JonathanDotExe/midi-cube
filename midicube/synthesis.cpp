/*
 * synthesis.cpp
 *
 *  Created on: 20 Jun 2020
 *      Author: jojo
 */

#include "synthesis.h"
#include <cmath>
#include <iostream>


extern double db_to_amp(double db) {
	return pow(10, db/10);
}

extern double note_to_freq_transpose (double tnote) {
	return pow(2, (tnote)/12.0);
}

extern double note_to_freq (double note) {
	return 440.0 * pow(2, (note - 69)/12.0);
}

extern double freq_to_radians(double freq) {
	return 2 * M_PI * freq;
}

extern double sine_wave(double time, double freq) {
	return sin(freq_to_radians(freq) * time);
}

extern double cosine_wave(double time, double freq) {
	return cos(freq_to_radians(freq) * time);
}


extern double square_wave(double time, double freq, double pulse_width) {
	return fmod(time, 1/freq) * freq >= pulse_width ? -1 : 1;
}

extern double saw_wave_down(double time, double freq) {
	double interval = 1.0/freq;
	return -fmod(time, interval)/interval * 2 + 1;
}

extern double saw_wave_up(double time, double freq) {
	double interval = 1.0/freq;
	return fmod(time, interval)/interval * 2 - 1;
}

/**
 * The arguments are irrelevant here, they are just there if a function pointer to a generic wave function is needed
 */
extern double noise_wave(double time, double freq) {
	return ((double) rand())/RAND_MAX* 2 - 1;
}


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
	std::cout << last_value << std::endl;
	last_time = time;
	this->value = value;
	this->slope_time = slope_time;
}


