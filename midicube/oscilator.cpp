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
double AnalogOscilator::signal(double time, double freq) {
	double signal = 0;
	switch(waveform) {
	case AnalogWaveForm::SINE:
		signal = sine_wave(time, freq);
		break;
	case AnalogWaveForm::SAW:
		signal = saw_wave(time, freq);
		break;
	case AnalogWaveForm::SQUARE:
		signal = square_wave(time, freq);
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

//AdditiveOscilator
double AdditiveOscilator::signal(double time, double freq) {
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

//SyncOscilator
double SyncOscilator::signal(double time, double freq) {
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


//OscilatorSlot
OscilatorSlot::OscilatorSlot(Oscilator* osc) {
	this->osc = osc;
	set_unison_detune();
}

double OscilatorSlot::signal(double time, double freq) {
	double signal = osc->signal(time, freq);
	double det = udetune;
	double ndet = nudetune;
	for (unsigned int i = 1; i <= unison; ++i) {
		signal += osc->signal(time, freq * det) + osc->signal(time, freq * ndet);
		det *= udetune;
		ndet *= nudetune;
	}
	return signal / (1 + 2 * unison);
}

unsigned int OscilatorSlot::get_unison() const {
	return unison;
}

void OscilatorSlot::set_unison(unsigned int unison = 0) {
	this->unison = unison;
}

double OscilatorSlot::get_unison_detune() const {
	return unison_detune;
}

void OscilatorSlot::set_unison_detune(double unison_detune = 0.1) {
	this->unison_detune = unison_detune;
	udetune = note_to_freq_transpose(unison_detune);
	nudetune = note_to_freq_transpose(-unison_detune);
}

double OscilatorSlot::get_volume() const {
	return volume;
}

void OscilatorSlot::set_volume(double volume = 1) {
	this->volume = volume;
}

OscilatorSlot::~OscilatorSlot() {
	delete osc;
	osc = nullptr;
}
