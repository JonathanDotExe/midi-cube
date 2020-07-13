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

	void add_sample(double sample, unsigned int delay, unsigned int repetition, unsigned int rep_delay, double factor);

	void add_sample(double sample, unsigned int delay);

	double process();

};

class LowPassFilter {

private:

	double last;
	double cutoff;
	double rc;

public:

	LowPassFilter(double cutoff = 21000);

	double apply (double sample, double time_step);

	void set_cutoff(double cutoff);

	double get_cutoff();
};

class HighPassFilter {

private:

	double lastFiltered;
	double last;
	double cutoff;
	double rc;
	bool started;

public:

	HighPassFilter(double cutoff = 21000);

	double apply (double sample, double time_step);

	void set_cutoff(double cutoff);

	double get_cutoff();
};

class BandPassFilter {

private:

	LowPassFilter lowpass;
	HighPassFilter highpass;

public:

	BandPassFilter(double low_cutoff = 21000, double high_cutoff = 0);

	double apply (double sample, double time_step);

	void set_low_cutoff(double cutoff);

	double get_low_cutoff();

	void set_high_cutoff(double cutoff);

	double get_high_cutoff();
};
#endif /* MIDICUBE_SYNTHESIS_H_ */
