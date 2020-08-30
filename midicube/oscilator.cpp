/*
 * oscilator.cpp
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#include "oscilator.h"
#include "synthesis.h"
#include <cmath>

Oscilator::Oscilator() {
	// TODO Auto-generated constructor stub

}

Oscilator::~Oscilator() {
	// TODO Auto-generated destructor stub
}

//AnalogOscilator
AnalogOscilator::AnalogOscilator(AnalogWaveForm waveform) {
	this->waveform = waveform;
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

double AnalogOscilator::signal(double time, double freq, double time_step) {
	double signal = 0;
	switch(waveform) {
	case AnalogWaveForm::SINE:
		signal = sine_wave(time, freq);
		break;
	case AnalogWaveForm::SAW:
		signal = saw_wave(time, freq);
		if (analog) {
			signal -= polyblep(time, freq, time_step);
		}
		break;
	case AnalogWaveForm::SQUARE:
		signal = square_wave(time, freq, pulse_width);
		if (analog) {
			signal += polyblep(time, freq, time_step);
			signal -= polyblep(time + pulse_width/freq, freq, time_step);
		}
		break;
	case AnalogWaveForm::NOISE:
		signal = noise_wave(time, freq);
		break;
	}
	return signal;
}

AnalogWaveForm AnalogOscilator::get_waveform() const {
	return waveform;
}

void AnalogOscilator::set_waveform(AnalogWaveForm waveform) {
	this->waveform = waveform;
}

AnalogOscilator::~AnalogOscilator() {

}

//OscilatorSlot
OscilatorSlot::OscilatorSlot(Oscilator* osc) {
	this->osc = osc;
	set_unison_detune();
}

double OscilatorSlot::signal(double time, double freq, double time_step) {
	double signal = osc->signal(time, freq, time_step);
	double det = udetune;
	double ndet = nudetune;
	for (unsigned int i = 1; i <= unison; ++i) {
		if (i % 2 == 0) {
			signal += osc->signal(time, freq * ndet, time_step);
			ndet *= nudetune;
		}
		else {
			signal += osc->signal(time, freq * det, time_step);
			det *= udetune;
		}
	}
	return signal / (1 + unison) * volume;
}

unsigned int OscilatorSlot::get_unison() const {
	return unison;
}

void OscilatorSlot::set_unison(unsigned int unison) {
	this->unison = unison;
}

double OscilatorSlot::get_unison_detune() const {
	return unison_detune;
}

void OscilatorSlot::set_unison_detune(double unison_detune) {
	this->unison_detune = unison_detune;
	udetune = note_to_freq_transpose(unison_detune);
	nudetune = note_to_freq_transpose(-unison_detune);
}

double OscilatorSlot::get_volume() const {
	return volume;
}

void OscilatorSlot::set_volume(double volume) {
	this->volume = volume;
}

OscilatorSlot::~OscilatorSlot() {
	delete osc;
	osc = nullptr;
}
