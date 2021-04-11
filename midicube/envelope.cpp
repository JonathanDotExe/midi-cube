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
	if (!pressed && !sustain && phase < RELEASE) {
		last_vol = volume;
		phase = RELEASE;
	}
	switch (phase) {
	case ATTACK:
		if (data.attack != 0 && data.peak_volume != 0) {
			volume += time_step/(data.attack * data.peak_volume);
		}
		else {
			volume =  data.peak_volume;
		}
		if (volume >= data.peak_volume) {
			volume = data.peak_volume;
			last_vol = volume;
			phase = DECAY;
		}
		break;
	case DECAY:
		if (data.decay != 0) {
			volume -= time_step/data.decay * (data.peak_volume - data.sustain);
		}
		else {
			volume = data.sustain;
		}

		if (volume <= data.sustain) {
			volume = data.sustain;
			last_vol = volume;
			time = 0;
			phase = SUSTAIN;
		}
		break;
	case SUSTAIN:
		if (data.sustain_time > 0) {
			time += time_step/data.sustain_time;
		}
		if (time >= 1) {
			last_vol = volume;
			phase = RELEASE;
		}
		break;
	case RELEASE:
		if (!sustain) {
			if (data.release != 0) {
				volume -= time_step/(data.release) * (last_vol - data.release_volume);
			}
			else {
				volume = data.release_volume;
			}
		}
		 if (volume <= data.release_volume) {
			 volume = data.release_volume;
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
	double s = 0;
	//Goto release phase
	if (!pressed && !sustain && phase < RELEASE) {
		last_vol = volume;
		phase = RELEASE;
		last = 0;
		time = 0;
	}

	switch (phase) {
	case ATTACK:
		step = time_step/data.attack;
		s = ANALOG_ADSR_WAVE.get_value(time) * data.peak_volume;
		if (data.attack != 0) {
			volume += s - last;
			last = s;
		}
		else {
			volume = data.peak_volume;
		}
		last = s;
		if (volume >= data.peak_volume) {
			volume = data.peak_volume;
			last_vol = volume;
			phase = DECAY;
			last = 0;
			time = 0;
			step = 0;
		}
		break;
	case DECAY:
		step = time_step/data.decay;
		s = ANALOG_ADSR_WAVE.get_value(time);
		if (data.decay != 0) {
			volume -= (s - last) * (data.peak_volume - data.sustain);
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
			step = 0;
		}
		break;
	case SUSTAIN:
		if (data.sustain_time > 0) {
			step = time_step/data.sustain_time;
		}
		if (time >= 1) {
			last_vol = volume;
			phase = RELEASE;
			last = 0;
			time = 0;
			step = 0;
		}
		break;
	case RELEASE:
		step = time_step/data.release;
		s = ANALOG_ADSR_WAVE.get_value(time);
		if (data.release != 0) {
			volume -= (s - last) * (last_vol - data.release_volume);
		}
		else {
			volume = 0;
		}

		last = s;
		if (volume <= data.release_volume) {
			volume = data.release_volume;
			last_vol = volume;
			phase = FINISHED;
			last = 0;
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


