/*
 * synthesis.h
 *
 *  Created on: 20 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SYNTHESIS_H_
#define MIDICUBE_SYNTHESIS_H_

#include <array>
#include <type_traits>
#include "audio.h"

#define DELAY_BUFFER_SIZE 1048576

extern double db_to_amp(double db);

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

class Filter {

public:

	virtual double apply (double sample, double time_step) = 0;

	virtual void set_cutoff(double cutoff) = 0;

	virtual double get_cutoff() = 0;

	virtual ~Filter() {

	}

};

class LowPassFilter : public Filter {

private:

	double last;
	double cutoff;
	double rc;

public:

	LowPassFilter(double cutoff = 21000);

	double apply (double sample, double time_step);

	void set_cutoff(double cutoff);

	double get_cutoff();

	~LowPassFilter() {

	}
};

class HighPassFilter : public Filter {

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

	~HighPassFilter() {

	}
};

class BandPassFilter : public Filter {

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

	void set_cutoff(double cutoff);

	double get_cutoff();

	~BandPassFilter() {

	}
};

template<typename T, typename = std::enable_if<std::is_base_of<Filter, T>::value>>
class MultiChannelFilter {
protected:
	std::array<T, OUTPUT_CHANNELS> channels;

public:

	void apply (std::array<double, OUTPUT_CHANNELS>& channels, double time_step) {
		for (std::size_t i = 0; i < this->channels.size(); ++i) {
			channels[i] = this->channels[i].apply(channels[i], time_step);
		}
	}

	std::array<T, OUTPUT_CHANNELS>& get_channels () {
		return channels;
	}

	void set_cutoff(double cutoff) {
		for (std::size_t i = 0; i < this->channels.size(); ++i) {
			this->channels[i].set_cutoff(cutoff);
		}
	}

	double get_cutoff() {
		return channels.at(0).get_cutoff();
	}

};

class MultiChannelBandPassFilter : public MultiChannelFilter<BandPassFilter> {

public:

	void set_low_cutoff(double cutoff) {
		for (std::size_t i = 0; i < this->channels.size(); ++i) {
			this->channels[i].set_low_cutoff(cutoff);
		}
	}

	double get_low_cutoff() {
		return channels.at(0).get_low_cutoff();
	}

	void set_high_cutoff(double cutoff) {
		for (std::size_t i = 0; i < this->channels.size(); ++i) {
			this->channels[i].set_low_cutoff(cutoff);
		}
	}

	double get_high_cutoff() {
		return channels.at(0).get_low_cutoff();
	}

};


#endif /* MIDICUBE_SYNTHESIS_H_ */
