/*
 * synthesis.h
 *
 *  Created on: 20 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SYNTHESIS_H_
#define MIDICUBE_SYNTHESIS_H_

#include <array>
#include <cmath>

#define DELAY_BUFFER_SIZE 1048576


constexpr double PI2{2 * M_PI};


inline extern double db_to_amp(double db) {
	return pow(10, db/10);
}

inline extern double note_to_freq_transpose (double tnote) {
	return pow(2, (tnote)/12.0);
}

inline extern double note_to_freq (double note) {
	return 440.0 * pow(2, (note - 69)/12.0);
}

inline extern double freq_to_radians(double freq) {
	return PI2 * freq;
}

inline extern double sine_wave(double phase) {
	return sin(PI2 * phase);
}

inline extern double integrated_sine_wave(double phase) {
	return -cos(PI2 * phase);
}


inline extern double cosine_wave(double phase) {
	return cos(PI2 * phase);
}


inline extern double square_wave(double phase, double pulse_width = 0.5) {
	return phase >= pulse_width ? -1 : 1;
}

inline extern double integrated_square_wave(double phase, double pulse_width = 0.5) {
	return phase >= pulse_width ? -phase : phase; //TODO might now work with pulse_width != 0.5
}


inline extern double saw_wave_down(double phase) {
	return -phase * 2 + 1;
}

inline extern double integrated_saw_wave_down(double phase) {
	return (-phase + 1) * phase;
}

inline extern double saw_wave_up(double phase) {
	return phase * 2 - 1;
}

inline extern double integrated_saw_wave_up(double phase) {
	return (phase - 1) * phase;
}

inline extern double triangle_wave(double phase, double pulse_width = 0.5) {
	if (phase < pulse_width) {
		return phase * 2 * 1/pulse_width - 1;
	}
	else {
		return -2 *(phase - 0.5)/(1 - pulse_width) + 1;
	}
}

inline extern double integrated_triangle_wave(double phase, double pulse_width = 0.5) {
	if (phase < pulse_width) {
		return (phase * 1/pulse_width - 1) * phase;
	}
	else {
		double inv_pw = 1 - pulse_width;
		return  (-phase/inv_pw + 1/inv_pw + 1) * phase;
	}
}

inline extern double noise_wave() {
	return ((double) rand())/RAND_MAX* 2 - 1;
}


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
