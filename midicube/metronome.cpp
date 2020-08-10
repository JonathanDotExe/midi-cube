/*
 * metronome.cpp
 *
 *  Created on: 10 Aug 2020
 *      Author: jojo
 */
#include "metronome.h"


Metronome::Metronome(int bpm) {
	this->bpm = bpm;
	this->start_time = 0;
}

void Metronome::init (unsigned int time) {
	this->start_time = time;
}

bool Metronome::is_beat(unsigned int time, unsigned int sample_rate, int value) {
	unsigned int step = time/sample_rate;
	return (time - start_time) % step == 0;
}

void Metronome::set_bpm(unsigned int bpm) {
	this->bpm = bpm;
}

unsigned int Metronome::get_bpm() {
	return bpm;
}
