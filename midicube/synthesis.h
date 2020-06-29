/*
 * synthesis.h
 *
 *  Created on: 20 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SYNTHESIS_H_
#define MIDICUBE_SYNTHESIS_H_

#include <array>

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

class DelayBuffer {
private:
	std::array<double, DELAY_BUFFER_SIZE> buffer = {};
	std::size_t index;

public:

	void add_sample(double sample, unsigned int delay);

	double process();

};

#endif /* MIDICUBE_SYNTHESIS_H_ */
