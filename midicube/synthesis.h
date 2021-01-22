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

extern double db_to_amp(double db);

extern double note_to_freq_transpose (double tnote);

extern double note_to_freq (double note);

extern double freq_to_radians(double freq);

extern double sine_wave(double time, double freq);

extern double cosine_wave(double time, double freq);

extern double square_wave(double time, double freq, double pulse_width = 0.5);

extern double saw_wave_down(double time, double freq);

extern double saw_wave_up(double time, double freq);

extern double triangle_wave(double time, double freq);

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

	void add_isample(double sample, double delay);

	double process();

};


class PortamendoBuffer {
private:
	double value;
	double last_value;
	double slope_time;
	double last_time;
public:
	PortamendoBuffer(double value, double slope_time);
	double get(double time);
	void set(double value, double time, double slope_time);
};


#endif /* MIDICUBE_SYNTHESIS_H_ */
