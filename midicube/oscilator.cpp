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

static double polyblep(double time, double freq, double time_step) {
	double phase_dur = 1/freq;
	time = fmod(time, phase_dur);
	if (time < time_step) {
		time /= time_step;
		return - time * time + 2 * time - 1;
	}
	else if (time > phase_dur - time_step) {
		time = (time - phase_dur)/time_step;
		return time * time + 2 * time + 1;
	}
	return 0;
}

double AnalogOscilator::signal(double time, double freq, double time_step, AnalogOscilatorData& data) {
	double signal = 0;
	switch(data.waveform) {
	case AnalogWaveForm::SINE:
		signal = sine_wave(time, freq);
		break;
	case AnalogWaveForm::SAW:
		signal = saw_wave(time, freq);
		if (data.analog) {
			signal -= polyblep(time, freq, time_step);
		}
		break;
	case AnalogWaveForm::SQUARE:
		signal = square_wave(time, freq, data.pulse_width);
		if (data.analog) {
			signal += polyblep(time, freq, time_step);
			signal -= polyblep(time + data.pulse_width/freq, freq, time_step);
		}
		break;
	case AnalogWaveForm::NOISE:
		signal = noise_wave(time, freq);
		break;
	}
	return signal;
}

AnalogOscilator::~AnalogOscilator() {

}

