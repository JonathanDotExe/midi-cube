/*
 * oscilator.cpp
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#include "oscilator.h"
#include <cmath>
#include <iostream>

//AnalogOscilator
AnalogOscilator::AnalogOscilator() {

}

void AnalogOscilator::process(double freq, double time_step, AnalogOscilatorData& data) {
	//Set freq to sync slave
	if (data.sync) {
		freq *= data.sync_mul;
	}
	//Move
	last_rotation = rotation;
	double step = freq * time_step;
	rotation += step;

	double phase = rotation - (long int) rotation;
	//Update parameters
	if (phase < step) {
		pulse_width = data.pulse_width;
	}
	//Sync
	/*if (sync_next) {
		sync_next = false;
		rotation = fmod(rotation, data.sync_mul);
	}
	else if (data.sync && rotation + step >= data.sync_mul) {
		sync_next = true;
		sync_phase = (data.sync_mul) - (long int) (data.sync_mul);
		if (sync_phase == 0) {
			sync_phase = 1;
		}
	}*/
	if (data.sync && rotation >= data.sync_mul) {
		rotation = fmod(rotation, data.sync_mul);
	}
}

double AnalogOscilator::carrier(double freq, double time_step, AnalogOscilatorData& data) {
	double phase = rotation - (long int) rotation;
	if (data.sync) {
		freq *= data.sync_mul;
	}
	double step = freq * time_step;
	//Compute wave
	double signal = 0;
	if (freq < 0.5/time_step) {
		switch(data.waveform) {
		case AnalogWaveForm::SINE_WAVE:
			//Modulator
			signal = sine_wave(phase);
			if (data.analog && data.sync) {
				double blep_amt = 0;
				/*if (sync_next || rotation < step) {
					blep_amt = -sine_wave(sync_phase)/2;
				}*/
				signal += polyblep(phase, step) * blep_amt;
			}
			break;
		case AnalogWaveForm::SAW_DOWN_WAVE:
			signal = saw_wave_down(phase);
			if (data.analog) {
				double blep_amt = 1;
				/*if (sync_next || rotation < step) {
					blep_amt = -saw_wave_down(sync_phase);
				}*/
				signal += polyblep(phase, step) * blep_amt;
			}
			break;
		case AnalogWaveForm::SAW_UP_WAVE:
			signal = saw_wave_up(phase);
			if (data.analog) {
				double blep_amt = 1;
				/*if (sync_next || rotation < step) {
					blep_amt = saw_wave_up(sync_phase);
				}*/
				signal -= polyblep(phase, step) * blep_amt;
			}
			break;
		case AnalogWaveForm::SQUARE_WAVE:
			signal = square_wave(phase, pulse_width);
			if (data.analog) {
				double blep_amt1 = 1;
				double blep_amt2 = 1;
				/*if (sync_next || rotation < step) {
					blep_amt1 = -square_wave(sync_phase, pulse_width);
					blep_amt2 = -blep_amt1;
				}*/
				signal += polyblep(phase, step) * blep_amt1;
				double protation = rotation + (1 - pulse_width);
				signal -= polyblep(protation - (long int) protation, step) * blep_amt2;
			}
			break;
		case AnalogWaveForm::TRIANGLE_WAVE:
			signal = triangle_wave(phase, pulse_width);
			if (data.analog) {
				double blep_amt1 = 1;
				double blep_amt2 = 1;
				/*if (sync_next || rotation < step) {
					blep_amt1 = -triangle_wave(sync_phase, pulse_width);
					blep_amt2 = -blep_amt1;
				}*/
				double mul1;
				double mul2;
				if (phase < pulse_width) {
					mul1 = 1/pulse_width;
					mul2 = 1/(1 - pulse_width);
				}
				else {
					mul1 = 1/(1 - pulse_width);
					mul2 = 1/pulse_width;
				}
				signal += integrated_polyblep(phase, step) * 2 * mul1 * step * blep_amt1;
				double protation = rotation + (1 - pulse_width);
				double pphase = protation - (long int) protation;
				signal -= integrated_polyblep(pphase, step) * 2 * mul2 * step * blep_amt2;
			}
			break;
		case AnalogWaveForm::NOISE_WAVE:
			signal = noise_wave();
			break;
		}
	}
	return signal;
}

double AnalogOscilator::modulator(double freq, double time_step, AnalogOscilatorData& data) {
	double phase = rotation - (long int) rotation;
	if (data.sync) {
		freq *= data.sync_mul;
	}
	//Compute wave
	//TODO antialiasing for integrated waves
	double signal = 0;
	if (freq < 0.5/time_step) {
		switch(data.waveform) {
		case AnalogWaveForm::SINE_WAVE:
			//Modulator
			signal = (integrated_sine_wave(phase) - integrated_sine_wave(fmod(last_rotation, 1)))/(time_step);
			break;
		case AnalogWaveForm::SAW_DOWN_WAVE:
			signal = (integrated_saw_wave_down(phase) - integrated_saw_wave_down(fmod(last_rotation, 1)))/(time_step);
			break;
		case AnalogWaveForm::SAW_UP_WAVE:
			signal = (integrated_saw_wave_up(phase) - integrated_saw_wave_up(fmod(last_rotation, 1)))/(time_step);
			break;
		case AnalogWaveForm::SQUARE_WAVE:
			signal = (integrated_square_wave(phase) - integrated_square_wave(fmod(last_rotation, 1)))/(time_step );
			break;
		case AnalogWaveForm::TRIANGLE_WAVE:
			signal = (integrated_triangle_wave(phase) - integrated_triangle_wave(fmod(last_rotation, 1)))/(time_step);
			break;
		case AnalogWaveForm::NOISE_WAVE:
			signal = 0;
			break;
		}
	}
	return signal;
}


void AnalogOscilator::reset(double phase) {
	rotation = phase;
}

void AnalogOscilator::randomize() {
	rotation = ((double) rand())/RAND_MAX;
}


AnalogOscilator::~AnalogOscilator() {

}

