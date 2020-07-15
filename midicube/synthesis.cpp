/*
 * synthesis.cpp
 *
 *  Created on: 20 Jun 2020
 *      Author: jojo
 */

#include "synthesis.h"
#include <cmath>


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
void DelayBuffer::add_sample(double sample, unsigned int delay) {
	if (delay < buffer.size()) {
		buffer[(index + delay) % buffer.size()] += sample;
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

//LowPassFilter
LowPassFilter::LowPassFilter(double cutoff) {
	this->cutoff = cutoff;
	this->last = 0;
	this->rc = 1.0/(2 * M_PI * cutoff);
}

double LowPassFilter::apply (double sample, double time_step) {
	double filtered = 0;
	double a = time_step / (rc + time_step);

	filtered = a * sample + (1 - a) * last;

	last = filtered;
	return filtered;
}

void LowPassFilter::set_cutoff(double cutoff) {
	this->cutoff = cutoff;
	this->rc = 1.0/(2 * M_PI * cutoff);
}

double LowPassFilter::get_cutoff() {
	return cutoff;
}

//HighPassFilter
HighPassFilter::HighPassFilter(double cutoff) {
	this->cutoff = cutoff;
	this->lastFiltered = 0;
	this->last = 0;
	this->rc = 1.0/(2 * M_PI * cutoff);
	this->started = false;
}

double HighPassFilter::apply (double sample, double time_step) {
	double filtered = 0;
	double a = rc / (rc + time_step);

	if (started) {
		filtered = a * lastFiltered + a * (sample - last);
	}
	else {
		filtered = sample;
		started = true;
	}

	last = sample;
	lastFiltered = filtered;
	return filtered;
}

void HighPassFilter::set_cutoff(double cutoff) {
	this->cutoff = cutoff;
	this->rc = 1.0/(2 * M_PI * cutoff);
}

double HighPassFilter::get_cutoff() {
	return cutoff;
}

//BandPassFilter
BandPassFilter::BandPassFilter(double low_cutoff, double high_cutoff) {
	set_low_cutoff(low_cutoff);
	set_high_cutoff(high_cutoff);
}

double BandPassFilter::apply (double sample, double time_step) {
	double filtered = lowpass.apply(highpass.apply(sample, time_step), time_step);
	return filtered;
}

void BandPassFilter::set_low_cutoff(double cutoff) {
	lowpass.set_cutoff(cutoff);
}

double BandPassFilter::get_low_cutoff() {
	return lowpass.get_cutoff();
}

void BandPassFilter::set_high_cutoff(double cutoff) {
	highpass.set_cutoff(cutoff);
}

double BandPassFilter::get_high_cutoff() {
	return highpass.get_cutoff();
}

void BandPassFilter::set_cutoff(double cutoff) {
	set_low_cutoff(cutoff);
	set_high_cutoff(cutoff);
}

double BandPassFilter::get_cutoff() {
	return (get_low_cutoff() + get_high_cutoff())/2.0;
}

