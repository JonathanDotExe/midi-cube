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
		signal = square_wave(time, freq);
		if (analog) {
			signal += polyblep(time, freq, time_step);
			signal -= polyblep(time + 1/(freq * 2), freq, time_step);
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

//AdditiveOscilator
AdditiveOscilator::AdditiveOscilator() {

}

double AdditiveOscilator::signal(double time, double freq, double time_step) {
	double sample = 0;
	double amp = 0;
	for (size_t i = 0; i < sines.size(); ++i) {
		sample += sine_wave(time, freq * sines[i].harmonic) * sines[i].amp;
		amp += sines[i].amp;
	}
	if (amp) {
		sample /= sines.size();
	}
	return sample;
}

void AdditiveOscilator::add_sine(AdditiveSine sine) {
	sines.push_back(sine);
}

AdditiveOscilator::~AdditiveOscilator() {

}

//SyncOscilator
SyncOscilator::SyncOscilator(AnalogWaveForm waveform, double detune) {
	this->waveform = waveform;
	this->detune = detune;
	properties.push_back("detune");
}

double SyncOscilator::signal(double time, double freq, double time_step) {
	double signal = 0;
	time = fmod(time, 1.0/freq);
	switch(waveform) {
	case AnalogWaveForm::SINE:
		signal = sine_wave(time, freq * detune);
		break;
	case AnalogWaveForm::SAW:
		signal = saw_wave(time, freq * detune);
		break;
	case AnalogWaveForm::SQUARE:
		signal = square_wave(time, freq * detune);
		break;
	case AnalogWaveForm::NOISE:
		signal = noise_wave(time, freq * detune);
		break;
	}
	return signal;
}

double SyncOscilator::get_detune() const {
	return detune;
}

AnalogWaveForm SyncOscilator::get_waveform() const {
	return waveform;
}

void SyncOscilator::set_waveform(AnalogWaveForm waveform) {
	this->waveform = waveform;
}

void SyncOscilator::set_detune(double detune) {
	this->detune = detune;
}

SyncOscilator::~SyncOscilator() {

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
