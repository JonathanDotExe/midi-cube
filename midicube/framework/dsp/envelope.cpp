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


double analog_adsr_wave(double prog) {
	return pow(prog, 2.0/3.0);
}

double linear_adsr_wave(double prog) {
	return prog;
}

double exponential_adsr_wave(double prog) {
	double val = prog >= 1 ? 1 : (1 - pow(pow(M_E, -8/44100.0), prog * 44100));
	return val;
}


WaveTable<2> LINEAR_ADSR_WAVE(linear_adsr_wave);
WaveTable<1024> ANALOG_ADSR_WAVE(analog_adsr_wave);
WaveTable<44101> EXPONENTIAL_ADSR_WAVE(exponential_adsr_wave);

inline double wavetable_adsr(double prog, ADSREnvelopeShape shape) {
	double val = 0;
	switch (shape) {
	case ADSREnvelopeShape::LINEAR_ADSR:
		val = prog;
		break;
	case ADSREnvelopeShape::ANALOG_ADSR:
		val = ANALOG_ADSR_WAVE.get_value(prog);
		break;
	case ADSREnvelopeShape::EXPONENTIAL_ADSR:
		val = EXPONENTIAL_ADSR_WAVE.get_value(prog);
		break;
	}
	return val;
}

template<typename T>
T signum(T t) {
	return (t > 0) - (t < 0);
}

//AnalogADSREnvelope
double WaveTableADSREnvelope::amplitude(ADSREnvelopeData& data, double time_step, bool pressed, bool sustain) {
	double step = 0;
	double s = 0;
	//Goto release phase
	if (!pressed && !sustain && phase < RELEASE) {
		last_vol = volume;
		phase = RELEASE;
		last = 0;
		time = 0;
	}

	double vol_before = volume;

	switch (phase) {
	case HOLD:
		if (data.hold > 0) {
			time += time_step/data.hold;
		}
		else {
			time = 1;
		}
		if (time >= 1) {
			last_vol = volume;
			phase = ATTACK;
			last = 0;
			time = 0;
			step = 0;
		}
		break;
	case ATTACK:
		step = time_step/data.attack;
		s = wavetable_adsr(time, data.attack_shape);
		if (data.attack != 0) {
			volume += (s - last);
			last = s;
		}
		else {
			volume = data.peak_volume;
			time = 1;
		}
		last = s;
		if (volume == data.peak_volume || (signum(data.peak_volume - volume) != signum(data.peak_volume - vol_before))) {
			volume = data.peak_volume;
			last_vol = volume;
			phase = ATTACK_HOLD;
			last = 0;
			time = 0;
			step = 0;
		}
		break;
	case ATTACK_HOLD:
		if (data.attack_hold > 0) {
			time += time_step/data.attack_hold;
		}
		else {
			time = 1;
		}
		if (time >= 1) {
			last_vol = volume;
			phase = PRE_DECAY;
			last = 0;
			time = 0;
			step = 0;
		}
		break;
	case PRE_DECAY:
		step = time_step/data.pre_decay;
		s = wavetable_adsr(time, data.pre_decay_shape);
		if (data.pre_decay != 0) {
			volume -= (s - last) * (data.peak_volume - data.decay_volume);
			last = s;
		}
		else {
			volume = data.decay_volume;
			time = 1;
		}

		last = s;
		if (volume == data.decay_volume || (signum(data.decay_volume - volume) != signum(data.decay_volume - vol_before))) {
			volume = data.decay_volume;
			last_vol = volume;
			phase = DECAY;
			last = 0;
			time = 0;
			step = 0;
		}
		break;
	case DECAY:
		step = time_step/data.decay;
		s = wavetable_adsr(time, data.decay_shape);
		if (data.decay != 0) {
			volume -= (s - last) * (data.decay_volume - data.sustain);
			last = s;
		}
		else {
			volume = data.sustain;
			time = 1;
		}

		last = s;
		if (volume == data.sustain || (signum(data.sustain - volume) != signum(data.sustain - vol_before))) {
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
		s = wavetable_adsr(time, data.release_shape);
		if (data.release != 0) {
			volume -= (s - last) * (last_vol - data.release_volume);
		}
		else {
			volume = 0;
			time = 1;
		}

		last = s;
		if (volume == data.release_volume || (signum(data.release_volume - volume) != signum(data.release_volume - vol_before))) {
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

	if (!sustain || phase != ADSREnvelopePhase::RELEASE || !data.pedal_catch) {
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
