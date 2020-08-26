/*
 * filter.h
 *
 *  Created on: 26 Aug 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_FILTER_H_
#define MIDICUBE_FILTER_H_
#include <array>
#include <type_traits>
#include "audio.h"

class RCFilter {

public:

	virtual double apply (double sample, double time_step) = 0;

	virtual void set_cutoff(double cutoff) = 0;

	virtual double get_cutoff() = 0;

	virtual ~RCFilter() {

	}

};

class RCLowPassFilter : public RCFilter {

private:

	double last;
	double cutoff;
	double rc;

public:

	RCLowPassFilter(double cutoff = 21000);

	double apply (double sample, double time_step);

	void set_cutoff(double cutoff);

	double get_cutoff();

	~RCLowPassFilter() {

	}
};

class RCHighPassFilter : public RCFilter {

private:

	double last_filtered;
	double last;
	double cutoff;
	double rc;
	bool started;

public:

	RCHighPassFilter(double cutoff = 21000);

	double apply (double sample, double time_step);

	void set_cutoff(double cutoff);

	double get_cutoff();

	~RCHighPassFilter() {

	}
};

class RCBandPassFilter : public RCFilter {

private:

	RCLowPassFilter lowpass;
	RCHighPassFilter highpass;

public:

	RCBandPassFilter(double cutoff = 10500, double bandwidth = 10500);

	double apply (double sample, double time_step);

	void set_cutoff(double cutoff);

	double get_cutoff();

	void set_bandwidth(double cutoff);

	double get_bandwidth();

	~RCBandPassFilter() {

	}
};



template<typename T, typename = std::enable_if<std::is_base_of<RCFilter, T>::value>>
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

class MultiChannelBandPassFilter : public MultiChannelFilter<RCBandPassFilter> {

public:

	void set_bandwidth(double bandwidth) {
		for (std::size_t i = 0; i < this->channels.size(); ++i) {
			this->channels[i].set_bandwidth(bandwidth);
		}
	}

	double get_bandwidth() {
		return channels.at(0).get_bandwidth();
	}

};


#endif /* MIDICUBE_FILTER_H_ */
