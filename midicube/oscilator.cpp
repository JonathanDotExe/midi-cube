/*
 * oscilator.cpp
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#include "oscilator.h"
#include <cmath>

//AnalogOscilator
AnalogOscilator::AnalogOscilator() {

}

static double polyblep(double phase, double step) {
	if (phase < step) {
		phase /= step;
		return - phase * phase + 2 * phase - 1;
	}
	else if (phase > 1 - step) {
		phase = (phase - 1)/step;
		return phase * phase + 2 * phase + 1;
	}
	return 0;
}

double AnalogOscilator::signal(double freq, double time_step, AnalogOscilatorData& data) {
	//Move
	rotation += freq * time_step;

	double phase = rotation - (long int) rotation;
	double step = freq * time_step;
	//Update parameters
	if (rotation - (long int) rotation < freq * time_step) {
		sync = data.sync;
		pulse_width = data.pulse_width;
	}
	//Sync
	double f = 1/sync;
	if (sync != 1) {
		if (phase >= f) {
			rotation += 1 - f;
			phase = rotation - (long int) rotation;
		}
	}
	//Compute wave
	double signal = 0;
	switch(data.waveform) {
	case AnalogWaveForm::SINE:
		signal = sine_wave(rotation, 1);
		if (sync != 1) {
			double sync_sig = sine_wave(f, 1); //TODO safe somewhere
			signal -= polyblep(phase * sync, step) * sync_sig;
		}
		break;
	case AnalogWaveForm::SAW:
		signal = saw_wave(rotation, 1);
		if (data.analog) {
			signal -= polyblep(phase * sync, step) * f;
		}
		break;
	case AnalogWaveForm::SQUARE:
		signal = square_wave(rotation, 1, pulse_width);
		if (data.analog) {
			signal += polyblep(phase * (sync >= pulse_width ? sync : 1), step);
			double protation = rotation + pulse_width/freq;
			signal -= polyblep(protation - (long int) protation, step);
		}
		break;
	case AnalogWaveForm::NOISE:
		signal = noise_wave(rotation, 1);
		break;
	}
	return signal;
}

AnalogOscilator::~AnalogOscilator() {

}

