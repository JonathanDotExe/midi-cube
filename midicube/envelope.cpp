/*
 * envelope.cpp
 *
 *  Created on: 1 Jul 2020
 *      Author: jojo
 */

#include "envelope.h"
#include "wavetable.h"
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

double analog_adsr_wave(double prog) {
	return pow(prog, 2.0/3.0);
}

WaveTable<1024> ANALOG_ADSR_WAVE(analog_adsr_wave);

//AnalogADSREnvelope
double AnalogADSREnvelope::amplitude(ADSREnvelopeData& data, double time_step, bool pressed, bool sustain) {
	double step = 0;
	double volume = 0;
	//Goto release phase
	if (!pressed && phase < RELEASE) {
		phase = RELEASE;
		time = 0;
		step = 0;
	}

	switch (phase) {
	case ATTACK:
		step = time_step/data.attack;
		volume = ANALOG_ADSR_WAVE.get_value(time);
		if (data.attack == 0 || volume >= 1) {
			volume = 1;
			phase = DECAY;
			time = 0;
			step = 0;
		}
		break;
	case DECAY:
		step = time_step/data.decay;
		volume = 1 - (ANALOG_ADSR_WAVE.get_value(time) * (1 - data.sustain));
		if (data.decay == 0 || volume <= data.sustain) {
			volume = data.sustain;
			phase = SUSTAIN;
			time = 0;
			step = 0;
		}
		break;
	case SUSTAIN:
		volume = data.sustain;
		break;
	case RELEASE:
		step = time_step/data.release;
		volume = (1 - ANALOG_ADSR_WAVE.get_value(time)) * data.sustain;
		if (data.release == 0 || volume <= 0) {
			volume = 0;
			phase = FINISHED;
			time = 0;
			step = 0;
		}
		break;
	case FINISHED:
		break;
	}

	if (!sustain || phase != ADSREnvelopePhase::RELEASE) {
		time += step;
	}

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


