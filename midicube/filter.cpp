/*
 * filter.cpp
 *
 *  Created on: 26 Aug 2020
 *      Author: jojo
 */


#include <cmath>
#include "filter.h"


double cutoff_to_rc(double cutoff) {
	return 1.0/(2 * M_PI * cutoff);
}

//LowPassFilter
RCLowPassFilter::RCLowPassFilter(double cutoff) {
	this->cutoff = cutoff;
	this->last = 0;
	this->rc = cutoff_to_rc(cutoff);
}

double RCLowPassFilter::apply (double sample, double time_step) {
	double filtered = 0;
	double a = time_step / (rc + time_step);

	filtered = a * sample + (1 - a) * last;

	last = filtered;
	return filtered;
}

void RCLowPassFilter::set_cutoff(double cutoff) {
	this->cutoff = cutoff;
	this->rc = cutoff_to_rc(cutoff);
}

double RCLowPassFilter::get_cutoff() {
	return cutoff;
}

//HighPassFilter
RCHighPassFilter::RCHighPassFilter(double cutoff) {
	this->cutoff = cutoff;
	this->last_filtered = 0;
	this->last = 0;
	this->rc = cutoff_to_rc(cutoff);
	this->started = false;
}

double RCHighPassFilter::apply (double sample, double time_step) {
	double filtered = 0;
	double a = rc / (rc + time_step);

	if (started) {
		filtered = a * last_filtered + a * (sample - last);
	}
	else {
		filtered = sample;
		started = true;
	}

	last = sample;
	last_filtered = filtered;
	return filtered;
}

void RCHighPassFilter::set_cutoff(double cutoff) {
	this->cutoff = cutoff;
	this->rc = cutoff_to_rc(cutoff);
}

double RCHighPassFilter::get_cutoff() {
	return cutoff;
}

//BandPassFilter
RCBandPassFilter::RCBandPassFilter(double cutoff, double bandwidth) {
	set_cutoff(cutoff);
	set_bandwidth(bandwidth);
}

double RCBandPassFilter::apply (double sample, double time_step) {
	double filtered = lowpass.apply(highpass.apply(sample, time_step), time_step);
	return filtered;
}

void RCBandPassFilter::set_cutoff(double cutoff) {
	double bandwidth = get_bandwidth();
	lowpass.set_cutoff(cutoff + bandwidth/2);
	highpass.set_cutoff(cutoff - bandwidth/2);
}

double RCBandPassFilter::get_cutoff() {
	return (lowpass.get_cutoff() + highpass.get_cutoff())/2.0;
}

void RCBandPassFilter::set_bandwidth(double bandwidth) {
	double cutoff = get_cutoff();
	lowpass.set_cutoff(cutoff + bandwidth/2);
	highpass.set_cutoff(cutoff - bandwidth/2);
}

double RCBandPassFilter::get_bandwidth() {
	return lowpass.get_cutoff() - highpass.get_cutoff();
}
