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
	double step = freq * time_step;
	rotation += freq * time_step;

	double phase = rotation - (long int) rotation;
	//Update parameters
	if (phase < step) {
		pulse_width = data.pulse_width;
	}
	//Sync
	double sync_step = data.sync_mul * freq * time_step;
	double sync_phase = 0;
	if (data.sync) {
		sync_rotation += sync_step;
		sync_phase = rotation - (long int) rotation;
		//Sync now
		if (sync_phase < sync_step) {
			//Reset phase
			rotation += 1 - phase + sync_step;
			phase = rotation - (long int) rotation;
		}
		//Sync next sample
		else if (sync_phase + sync_step >= 1) {
			last_phase = phase;
		}
	}
	//Compute wave
	double signal = 0;
	switch(data.waveform) {
	case AnalogWaveForm::SINE:
		signal = sine_wave(rotation, 1);
		if (data.analog && data.sync && data.sync_mul != 1) {
			signal -= polyblep(sync_phase, sync_step) * (sine_wave(last_phase, 1) + 1) / 2;
		}
		break;
	case AnalogWaveForm::SAW_DOWN:
		signal = saw_wave_down(rotation, 1);
		if (data.analog) {
			signal += polyblep(phase, step);
			if (data.sync && data.sync_mul != 1) {
				signal += polyblep(sync_phase, sync_step) * (1 - (saw_wave_down(last_phase, 1) + 1) / 2);
			}
		}
		break;
	case AnalogWaveForm::SAW_UP:
		signal = saw_wave_up(rotation, 1);
		if (data.analog) {
			signal -= polyblep(phase, step);
			if (data.sync && data.sync_mul != 1) {
				signal += polyblep(sync_phase, sync_step) * (saw_wave_up(last_phase, 1) + 1) / 2;
			}
		}
		break;
	case AnalogWaveForm::SQUARE:
		signal = square_wave(rotation, 1, pulse_width);
		if (data.analog) {
			signal += polyblep(phase, step);
			double protation = rotation + pulse_width;
			signal -= polyblep(protation - (long int) protation, step);

			if (data.sync && data.sync_mul != 1) {
				signal += polyblep(sync_phase, sync_step) *  (1 - (square_wave(rotation, 1, pulse_width) + 1) / 2);
			}
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

