/*
 * synthesis.h
 *
 *  Created on: 20 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SYNTHESIS_H_
#define MIDICUBE_SYNTHESIS_H_

#include "util.h"
#include <vector>

#define DELAY_BUFFER_SIZE 1048576

extern double note_to_freq_transpose (double tnote);

extern double note_to_freq (double note);

extern double freq_to_radians(double freq);

extern double sine_wave(double time, double freq);

extern double square_wave(double time, double freq);

extern double saw_wave(double time, double freq);

/**
 * The arguments are irrelevant here, they are just there if a function pointer to a generic wave function is needed
 */
extern double noise_wave(double time, double freq);

struct DelaySample {
	double time;
	double sample;
};

class DelayBuffer {
private:
	CircularBuffer<DelaySample, DELAY_BUFFER_SIZE> buffer;

public:

	void add_sample(DelaySample sample);

	double process(double time);

};

class SortedDelayBuffer {
private:
	std::vector<DelaySample> buffer;

public:

	SortedDelayBuffer();

	void add_sample(DelaySample sample);

	double process(double time);

};

#endif /* MIDICUBE_SYNTHESIS_H_ */
