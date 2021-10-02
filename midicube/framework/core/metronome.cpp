/*
 * metronome.cpp
 *
 *  Created on: 10 Aug 2020
 *      Author: jojo
 */
#include "metronome.h"

#include "math.h"

Metronome::Metronome(int bpm) {
	this->bpm = bpm;
	this->start_time = 0;
}

void Metronome::init (unsigned int time) {
	this->start_time = time;
}

bool Metronome::is_beat(unsigned int time, unsigned int sample_rate, double value) const {
	if (!bpm) {
		return time - start_time == 0;
	}
	double step = 60.0 * sample_rate/(value * bpm);
	return fmod((time - start_time), step) < 1;
}

double Metronome::last_beat(unsigned int time, unsigned int sample_rate, double value) const {
	if (!bpm) {
		return 0;
	}
	double step = 60.0 * sample_rate/(value * bpm);
	return ((time - start_time) / step) * step + start_time;
}

void Metronome::set_bpm(unsigned int bpm) {
	this->bpm = bpm;
}

unsigned int Metronome::get_bpm() const {
	return bpm;
}

double Metronome::beats(unsigned int time, unsigned int sample_rate, double value) const {
	double step = 60.0 * sample_rate/(value * bpm);
	return (time - start_time)/step;
}
