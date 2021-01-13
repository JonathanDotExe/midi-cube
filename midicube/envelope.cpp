/*
 * envelope.cpp
 *
 *  Created on: 1 Jul 2020
 *      Author: jojo
 */

#include "envelope.h"
#include <cmath>
#include <iostream>

//ADSREnvelope
double ADSREnvelope::amplitude(ADSREnvelopeData& data, double time_step, bool pressed, bool sustain) {
	//Goto release phase
	if (!pressed && phase < RELEASE) {
		last_vol = volume;
		phase = RELEASE;
	}
	switch (phase) {
	case ATTACK:
		if (data.attack != 0) {
			volume += time_step/data.attack;
		}
		else {
			volume = 1;
		}
		if (volume >= 1) {
			volume = 1;
			last_vol = volume;
			phase = DECAY;
		}
		break;
	case DECAY:
		if (data.decay != 0) {
			volume -= time_step/data.decay * (1 - data.sustain);
		}
		else {
			volume = data.sustain;
		}

		if (volume <= data.sustain) {
			volume = data.sustain;
			last_vol = volume;
			phase = SUSTAIN;
		}
		break;
	case SUSTAIN:
		break;
	case RELEASE:
		if (!sustain) {
			if (data.release != 0) {
				volume -= time_step/data.release * last_vol;
			}
			else {
				volume = 0;
			}
		}
		 if (volume <= 0) {
			 volume = 0;
			 last_vol = volume;
			 phase = FINISHED;
		 }
		break;
	case FINISHED:
		break;
	}
	return volume;
}

//EnvelopeFollower
EnvelopeFollower::EnvelopeFollower(double step_time) {
	this->step_time = step_time;
	port.set(0, 0, step_time);
}

void EnvelopeFollower::apply(double signal, double time) {
	double val = volume(time);
	signal = fabs(signal);
	port.set(signal, time, signal >= val ? 0 : step_time * (val - signal));
}

double EnvelopeFollower::volume(double time) {
	return port.get(time);
}

void EnvelopeFollower::set_step_time(double step_time) {
	this->step_time = step_time;
}


