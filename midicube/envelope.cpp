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
	if (!pressed && phase != FINISHED) {
		phase = RELEASE;
	}
	switch (phase) {
	case ATTACK:
		volume += time_step/data.attack;
		if (volume >= 1) {
			volume = 1;
			phase = DECAY;
		}
		break;
	case DECAY:
		volume -= time_step/data.decay * (1 - data.sustain);
		if (volume <= data.sustain) {
			volume = data.sustain;
			phase = SUSTAIN;
		}
		break;
	case SUSTAIN:
		break;
	case RELEASE:
		if (!sustain) {
			if (data.release != 0 && data.sustain != 0) {
				volume -= time_step/data.release * data.sustain;
			}
			else {
				volume = 0;
			}
		}
		 if (volume <= 0) {
			 volume = 0;
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
}

void EnvelopeFollower::apply(double signal, double time, double time_step) {
	buffer[index] = signal;
	if (std::abs(signal) >= highest_signal) {
		highest_signal = std::abs(signal);
		highest_time = time;
	}
	else if (highest_time < time - step_time) {
		highest_signal = -1;
		//Look for new highest signal
		size_t timespan = round(step_time / time_step);
		for (size_t i = 0; i < timespan; ++i) {
			size_t pos = (index - i)%buffer.size();
			if (std::abs(buffer[pos]) >= highest_signal) {
				highest_signal = std::abs(buffer[pos]);
				highest_time = time - time_step * i;
			}
		}
	}

	index = (index + 1) % buffer.size();
}

double EnvelopeFollower::volume() {
	return highest_signal;
}

void EnvelopeFollower::set_step_time(double step_time) {
	this->step_time = step_time;
}


