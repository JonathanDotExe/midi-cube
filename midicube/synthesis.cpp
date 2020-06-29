/*
 * synthesis.cpp
 *
 *  Created on: 20 Jun 2020
 *      Author: jojo
 */

#include "synthesis.h"
#include <cmath>


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

extern double square_wave(double time, double freq) {
	return copysign(1.0, sin(freq_to_radians(freq) * time));
}

extern double saw_wave(double time, double freq) {
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
void DelayBuffer::add_sample(DelaySample sample) {
	buffer.push_back(sample);
}

double DelayBuffer::process(double time) {
	double sample = 0;
	while (buffer.size() > 0) {
		DelaySample samp = buffer.peek_first();
		if (samp.time >= time) {
			buffer.pop_first();
			sample += samp.sample;
		}
		else {
			break;
		}
	}
	return sample;
}


