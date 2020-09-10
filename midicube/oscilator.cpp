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

static double polyblep(double rotation, double freq, double time_step) {
	double phase = rotation - (long int) rotation;
	double step = freq * time_step;
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

	//Compute wave
	double signal = 0;
	double detune = note_to_freq_transpose(data.unison_detune);
	double ndetune = note_to_freq_transpose(-data.unison_detune);
	for (size_t i = 0; i <= data.unison_amount; ++i) {
		double det = i % 2 == 0 ? (ndetune * (int) ((i + 1)/2)) : (detune * (int) ((i + 1)/2));
		switch(data.waveform) {
		case AnalogWaveForm::SINE:
			signal += sine_wave(rotation * det, 1);
			break;
		case AnalogWaveForm::SAW:
			signal += saw_wave(rotation * det, 1);
			if (data.analog) {
				signal -= polyblep(rotation, freq, time_step);
			}
			break;
		case AnalogWaveForm::SQUARE:
			signal += square_wave(rotation * det, 1, data.pulse_width);
			if (data.analog) {
				signal += polyblep(rotation, freq, time_step);
				signal -= polyblep(rotation + data.pulse_width/freq, freq, time_step);
			}
			break;
		case AnalogWaveForm::NOISE:
			signal += noise_wave(rotation * det, 1);
			break;
		}
	}
	return signal / (data.unison_amount + 1);
}

AnalogOscilator::~AnalogOscilator() {

}

