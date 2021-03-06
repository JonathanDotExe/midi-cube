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
double LinearADSREnvelope::amplitude(ADSREnvelopeData& data, double time_step, bool pressed, bool sustain) {
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

//AnalogADSREnvelope
double AnalogADSREnvelope::amplitude(ADSREnvelopeData& data, double time_step, bool pressed, bool sustain) {
	double step = 0;
	double s = 0;
	//Goto release phase
	if (!pressed && phase < RELEASE) {
		last_vol = volume;
		phase = RELEASE;
		last = 0;
		time = 0;
	}

	switch (phase) {
	case ATTACK:
		step = time_step/data.attack;
		s = pow(time, slope);
		if (data.attack != 0) {
			volume += s - last;
			last = s;
		}
		else {
			volume = 1;
		}
		last = s;
		if (volume >= 1) {
			volume = 1;
			last_vol = volume;
			phase = DECAY;
			last = 0;
			time = 0;
		}
		break;
	case DECAY:
		step = time_step/data.decay;
		s = pow(time, slope);
		if (data.decay != 0) {
			volume -= (s - last) * (1 - data.sustain);
			last = s;
		}
		else {
			volume = data.sustain;
		}

		last = s;
		if (volume <= data.sustain) {
			volume = data.sustain;
			last_vol = volume;
			phase = SUSTAIN;
			last = 0;
			time = 0;
		}
		break;
	case SUSTAIN:
		break;
	case RELEASE:
		step = time_step/data.release;
		s = pow(time, slope);
		if (!sustain) {
			if (data.release != 0) {
				volume -= (s - last) * last_vol;
			}
			else {
				volume = 0;
			}
		}

		last = s;
		if (volume <= 0) {
			volume = 0;
			last_vol = volume;
			phase = FINISHED;
			last = 0;
			time = 0;
		}
		break;
	case FINISHED:
		break;
	}

	time += step;

	return volume;
}

//EnvelopeFollower
EnvelopeFollower::EnvelopeFollower() {

}

void EnvelopeFollower::apply(double signal, double time_step) {
	signal = fabs(signal);
	value = filter.apply(data, signal, time_step);
}

double EnvelopeFollower::volume() {
	return value;
}


